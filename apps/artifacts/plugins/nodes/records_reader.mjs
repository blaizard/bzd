import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import zlib from "zlib";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// Concurrent lock-free reader of records on disk.
export default class RecordsReader {
	/// Construct on-disk iterable records.
	///
	/// \param options Configuration options for the Record instance.
	/// - path (string, required): The base directory where record storages are located.
	/// - fs: The file system module to use.
	/// - statistics: The statistics object to be used.
	constructor(options = {}) {
		this.options = Object.assign(
			{
				path: null,
				fs: FileSystem,
				statistics: new StatisticsProvider("records-reader"),
			},
			options,
		);
		Exception.assert(this.options.path !== null, "'path' must be set.");
		this.storages = {};
	}

	/// The current version of the records, any records with a different version
	/// will be discarded.
	static get version() {
		return 2;
	}

	/// Get the tick from a path.
	static getTickFromPath(path) {
		const filePattern = new RegExp("^(?:.*/)?([0-9]+)\\.rec(\\.gz)?$");
		const match = filePattern.exec(path);
		return match === null ? null : parseInt(match[1]);
	}

	/// Get payload of a record into a list.
	static payloadToList(payload) {
		const data = JSON.parse(payload + "[]]}");
		Exception.assert(
			data.version == RecordsReader.version,
			"Record is from an incompatible version: {} vs {}.",
			data.version,
			RecordsReader.version,
		);
		return data.records.slice(0, -1);
	}

	async init() {
		await this.discoverAll();
	}

	async discoverAll() {
		const storages = await this.options.fs.readdir(this.options.path, /*withFileTypes*/ true);
		for (const entry of storages) {
			if (entry.isDirectory()) {
				await this.discover(entry.name);
			}
		}
	}

	async discover(storageName) {
		const storagePath = this.options.path + "/" + storageName;
		do {
			const entries = await this.options.fs.readdir(storagePath, /*withFileTypes*/ true);
			let records = [];
			for (const entry of entries) {
				if (entry.isFile()) {
					const path = storagePath + "/" + entry.name;
					const maybeTick = RecordsReader.getTickFromPath(path);
					// Invalid file pattern, ignore
					if (maybeTick === null) {
						continue;
					}
					records.push({
						tick: maybeTick,
						path: path,
					});
				}
			}
			records.sort((a, b) => a.tick - b.tick);
			try {
				const stats = await Promise.all(records.map((record) => this.options.fs.stat(record.path)));
				records = records.map((record, index) =>
					Object.assign(record, {
						hash: stats[index].size,
					}),
				);
			} catch (e) {
				// If it fails it means that a file have been deleted meanwhile.
				continue;
			}

			this.storages[storageName] = {
				records: records,
				hash: String(records.length) + "." + String(records.reduce((acc, entry) => acc + entry.tick, 0)),
			};
		} while (false);
	}

	/// Read the content of a record and handle decompression if needed.
	async readRecord(path) {
		if (path.endsWith(".rec")) {
			this.options.statistics.sum("read", 1);
			return await this.options.fs.readFile(path);
		} else if (path.endsWith(".gz")) {
			this.options.statistics.sum("read.gz", 1);
			const content = await this.options.fs.readBinary(path);
			return new Promise((resolve, reject) => {
				zlib.gunzip(content, (err, decompressedBuffer) => {
					if (err) {
						reject(new Exception("Error decompressing data: {}", err));
						return;
					}
					resolve(decompressedBuffer.toString("utf8"));
				});
			});
		}
		Exception.unreachable("Unsupported file format: {}", path);
	}

	/// Read a record from a specific storage at a given tick to its end.
	///
	/// \param storage The storage to read from.
	/// \param tick Initial tick.
	///
	/// \return A tuple, containing the list of payload previously stored, and the next tick.
	///         In case there are no new payloads, null is returned.
	async _readStorageByChunk(storageName, tick) {
		/// If the tick is null, return null, this is to ease looping over _read.
		if (tick === null) {
			return null;
		}

		const fetchEntries = async () => {
			let memoization = {};
			let recordsProcessed = 0;

			do {
				// Handle case when the records are empty or if some records have been processed
				// previously, it means that some data are not available.
				if (!(storageName in this.storages) || this.storages[storageName].records.length == 0 || recordsProcessed > 0) {
					await this.discover(storageName);
				}

				const storage = this.storages[storageName];

				// This storage is not existing anymore.
				if (storage === undefined) {
					break;
				}

				// Look for the entry.
				let index = storage.records.findLastIndex((entry) => entry.tick <= tick);
				if (index === -1) {
					index = 0;
				}

				recordsProcessed = 0;
				for (; index < storage.records.length; ++index) {
					const entry = storage.records[index];

					// Check if it was already processed.
					if (memoization[entry.path] == entry.hash) {
						continue;
					}
					memoization[entry.path] = entry.hash;
					++recordsProcessed;

					// Read the file content.
					try {
						const content = await this.readRecord(entry.path);
						const payloads = RecordsReader.payloadToList(content);
						const indexPayload = payloads.findIndex((payloadEntry) => payloadEntry[0] >= tick);
						if (indexPayload !== -1) {
							return payloads.slice(indexPayload);
						}
					} catch (e) {
						// An exception means that the file might have been written or doesn't exists anymore.
						break;
					}

					// No match, we are either at the end of the records or we need to look for the next record.
				}
			} while (recordsProcessed > 0);

			return null;
		};

		const maybeEntries = await fetchEntries();
		if (maybeEntries === null) {
			return null;
		}

		Exception.assert(maybeEntries.length > 0, "The output cannot be empty as we search in it just before.");
		return maybeEntries;
	}

	async *_readStorage(storageName, tick = 0) {
		let result = null;
		while ((result = await this._readStorageByChunk(storageName, tick))) {
			let payload = null;
			for (payload of result) {
				yield payload;
			}
			tick = payload[0] + 1;
		}
	}

	/// Generator to read all new values starting from a specific tick.
	///
	/// \param tick The initial tick.
	///
	/// \return A tuple consisting of:
	///         1. The next tick to get the new value.
	///         2. The value.
	///         3. The approximated size of the serialized payload.
	async *read(tick = 0) {
		// Load the first value of all records and filter the ones without values.
		let iterators = Object.keys(this.storages).map((storageName) => ({
			gen: this._readStorage(storageName, tick),
			result: null,
		}));
		await Promise.all(
			iterators.map(async (it) => {
				it.result = await it.gen.next();
			}),
		);
		iterators = iterators.filter((it) => !it.result.done);

		// Helper.
		const getMinIndex = () =>
			iterators.reduce(
				(minIndex, it, i, array) => (it.result.value[1] < array[minIndex].result.value[1] ? i : minIndex),
				0,
			);

		while (iterators.length > 0) {
			const index = getMinIndex();

			yield iterators[index].result.value;

			// Get the next value.
			iterators[index].result = await iterators[index].gen.next();
			if (iterators[index].result.done) {
				iterators.splice(index, 1);
			}
		}
	}
}
