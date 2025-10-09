import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import Lock from "#bzd/nodejs/core/lock.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import zlib from "zlib";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";
import RecordsReader from "#bzd/apps/artifacts/plugins/nodes/records_reader.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// Construct on-disk iterable records.
///
/// The format of each entry is as follow:
/// [tick, payload]
export default class Record {
	/// Construct on-disk iterable records.
	///
	/// \param options Configuration options for the Record instance.
	/// - path (string, required): The base directory where record storages will be created.
	/// - fs: The file system module to use.
	/// - recordMaxSize: The maximum size of a single record file in bytes.
	/// - maxSize: The maximum total size of all records in a storage in bytes.
	/// - storages: An array of storage names to create.
	/// - clean: Whether the directory should be cleaned or not.
	/// - statistics: The statistics object to be used.
	constructor(options = {}) {
		this.options = Object.assign(
			{
				path: null,
				fs: FileSystem,
				/// Maximum size of a single record not compressed.
				recordMaxSize: 1024 * 1024,
				/// Maximum size of all the records compressed.
				maxSize: 20 * 1024 * 1024,
				storages: [Record.defaultStorageName],
				/// If set all previous records will be removed.
				clean: false,
				statistics: new StatisticsProvider("records"),
			},
			options,
		);
		Exception.assert(this.options.path !== null, "'path' must be set.");
		this.storages = {};
		for (const storageName of this.options.storages) {
			this.makeStorage(storageName);
		}
		this.tick = 0;
		this.reader = new RecordsReader(this.options);

		Exception.assert(this.options.recordMaxSize < this.options.maxSize);
	}

	/// Create a new record storage.
	makeStorage(name) {
		Exception.assert(!(name in this.storages), "Storage named '{}' already exists.", name);
		this.storages[name] = {
			path: this.options.path + "/" + name,
			lock: new Lock(),
			records: null,
			tickRemote: null,
			statistics: this.options.statistics.makeNested("storage", name),
		};
	}

	/// The current version of the records, any records with a different version
	/// will be discarded.
	static get version() {
		return RecordsReader.version;
	}

	/// The name of the default storage.
	static get defaultStorageName() {
		return "main";
	}

	async init(callback) {
		// Cleanup the directory if needed.
		if (this.options.clean) {
			await this.options.fs.rmdir(this.options.path, /*mustExists*/ false);
		}

		// Initialize all storages.
		for (const [storageName, storage] of Object.entries(this.storages)) {
			await this.options.fs.mkdir(storage.path);

			// Initialize the files map.
			const files = await this.options.fs.readdir(storage.path);
			storage.records = [];
			for (const name of files) {
				const path = storage.path + "/" + name;
				const maybeTick = RecordsReader.getTickFromPath(path);
				// Invalid file pattern, try to delete the file.
				if (maybeTick === null) {
					Log.error("Invalid record, doesn't match the file pattern: '{}', deleting.", path);
					await this.options.fs.unlink(path);
					continue;
				}
				await this._addRecordEntry(storage, maybeTick, path);
			}

			const [tick, tickRemote] = await this._sanitizeStorage(storage, callback);
			storage.tickRemote = tickRemote;
			this.tick = Math.max(this.tick, tick);

			Log.info("Initialized storage '{}' with {} record(s).", storageName, storage.records.length);
		}

		// Initialize the reader after the storage is set.
		await this.reader.init();

		return {
			storages: Object.fromEntries(
				Object.entries(this.storages).map(([name, storage]) => [
					name,
					{ files: storage.records.length, tickRemote: storage.tickRemote },
				]),
			),
			tick: this.tick,
		};
	}

	/// Get the latest remote tick.
	getTickRemote(storageName = Record.defaultStorageName, defaultValue = null) {
		Exception.assert(storageName in this.storages, "Invalid storage requested: '{}'.", storageName);
		const tick = this.storages[storageName].tickRemote;
		if (tick === null) {
			return defaultValue;
		}
		return tick;
	}

	/// Register a new entry to the records.
	///
	/// \param storage The storage to add the entry to.
	/// \param tick The tick related to the file to be created.
	/// \param path The path of the existing record if any, null otherwise.
	async _addRecordEntry(storage, tick, path = null) {
		// Look for the index that shall be just before the one to insert.
		const index = storage.records.findLastIndex((entry) => entry.tick <= tick);
		Exception.assert(index === -1 || storage.records[index].tick < tick, "2 ticks cannot have the same value.");

		// Create the file if it does not exists.
		if (path === null) {
			path = storage.path + "/" + tick + ".rec";
			await this.options.fs.touch(path);
			// Add the header.
			await this.options.fs.appendFile(path, '{"version":' + Record.version + ',"records":[');
		}

		// Create and insert the new entry.
		const entry = {
			tick: tick,
			path: path,
			size: (await this.options.fs.stat(path)).size,
		};
		storage.records.splice(index + 1, 0, entry);

		// Cleanup previous records if needed.
		let size = storage.records.reduce((total, record) => record.size + total, 0);
		while (storage.records.length && size > this.options.maxSize - this.options.recordMaxSize) {
			const entry = storage.records.shift();
			await this.options.fs.unlink(entry.path);
			size -= entry.size;
		}

		storage.statistics.set("size", size);
		storage.statistics.set("files", storage.records.length);

		return entry;
	}

	/// Compress an existing entry and update the entry itself.
	async _compressEntry(entry) {
		const content = await this.reader.readRecord(entry.path);
		const compressed = await new Promise((resolve, reject) => {
			zlib.gzip(content, (err, compressedBuffer) => {
				if (err) {
					reject(new Exception("Error compressing data: {}", err));
					return;
				}
				resolve(compressedBuffer);
			});
		});
		const newPath = entry.path + ".gz";
		await this.options.fs.writeBinary(newPath, compressed);
		await this.options.fs.unlink(entry.path);

		// Update the entry information.
		entry.path = newPath;
		entry.size = (await this.options.fs.stat(newPath)).size;
	}

	/// Add new payload to the record list.
	async _getLastEntryForWriting(storage, tick, payloadSize) {
		Exception.assertPrecondition(storage.records !== null, "Used before initialization.");
		let entry = storage.records[storage.records.length - 1] || null;

		// Check if the entry has still enough space, if not compress it.
		if (entry && entry.size + payloadSize > this.options.recordMaxSize) {
			await this._compressEntry(entry);
			entry = null;
		}

		// Check if this is a compressed entry, in that case skip it.
		if (entry && !entry.path.endsWith(".rec")) {
			entry = null;
		}

		// There is no available entry, create a new one.
		if (entry === null) {
			entry = await this._addRecordEntry(storage, tick);
		}

		return entry;
	}

	/// Write new records.
	///
	/// \param payload The payload to be stored.
	/// \param storageName The name of the storage.
	/// \param tickRemote The latest remote tick that leads to new elements.
	async write(payload, storageName = Record.defaultStorageName, tickRemote = null) {
		const serialized = JSON.stringify(payload);
		Exception.assert(storageName in this.storages, "Invalid storage requested: '{}'.", storageName);
		const storage = this.storages[storageName];

		// Get the record and write to it atomically.
		await storage.lock.acquire(async () => {
			const tick = ++this.tick;
			const content = "[" + tick + "," + serialized + "," + serialized.length + "," + tickRemote + "],\n";
			const entry = await this._getLastEntryForWriting(storage, tick, content.length);
			await this.options.fs.appendFile(entry.path, content);
			entry.size += content.length;
			storage.tickRemote = tickRemote;
		});
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
		yield* this.reader.read(tick);
	}

	async _delete(storage, record) {
		for (let index = 0; index < storage.records.length; ++index) {
			if (storage.records[index].path == record.path) {
				Log.info("Deleting record '{}'.", record.path);
				storage.records.splice(index, 1);
				await this.options.fs.unlink(record.path);
				return;
			}
		}
		Exception.unreachable("The record '{}' was not found.", record.path);
	}

	/// Ensure that everything is in order and reset the tick count.
	/// Also remove invalid records if any.
	///
	/// Things that are checked.
	/// - All records files name have increasing ticks and are sorted.
	/// - All records ticks are increasing.
	/// - The size of each file doesn't exceed the maximum size (unless it contains a single record).
	/// - The size of all records doesn't exceed the maximum size (unless all entries are single).
	///
	/// \param storage The storage to be used.
	/// \param callback A callback that will be called on each record.
	///
	/// \return A tuple containing the current tick and the current remote tick (if any).
	async _sanitizeStorage(storage, callback) {
		let tick = null;
		let remoteTick = null;
		let totalSize = 0;
		let allSingleEntries = true;
		let toDelete = [];
		for (const record of storage.records) {
			const maybeTick = RecordsReader.getTickFromPath(record.path);
			Exception.assert(maybeTick !== null, "Tick cannot be extracted from path '{}'.", record.path);
			Exception.assert(record.tick == maybeTick, "Path '{}' and tick '{}' do not match.", record.path, record.tick);
			if (tick !== null) {
				Exception.assert(
					record.tick > tick,
					"Tick of the next record must greater: expected > {} but got {}",
					tick,
					record.tick,
				);
			}

			tick = record.tick - 1;

			try {
				// Check the payloads
				const content = await this.reader.readRecord(record.path);
				// Only check if it is not compressed
				if (record.path.endsWith(".rec")) {
					Exception.assert(
						content.length == record.size,
						"There is a mismatch between the payload size and the size recorded: {} vs {}.",
						content.length,
						record.size,
					);
				}
				const entries = RecordsReader.payloadToList(content);

				for (const [t, entry, _, remoteT] of entries) {
					Exception.assert(t > tick, "Ticks are increasing, expected > {} but got {}", tick, t);
					if (remoteT !== null && remoteTick !== null && remoteT < remoteTick) {
						Log.warning(
							"Remote ticks are increasing, expected >= {} but got {}. This happens if remote restarted and lost its previous records.",
							remoteTick,
							remoteT,
						);
					}
					tick = t;
					remoteTick = remoteT;
					if (callback) {
						callback(entry);
					}
				}

				// Only check the size if there are more than 1 entry.
				// With one entry, this condition can be false if the entry is larger than the actual max size.
				if (entries.length > 1) {
					Exception.assert(
						record.size <= this.options.recordMaxSize,
						"The record size is too large: {} vs {}",
						record.size,
						this.options.recordMaxSize,
					);
				}

				// Update only if the record is successful.
				allSingleEntries &= entries.length == 1;
				totalSize += record.size;
			} catch (e) {
				Log.error("Ignoring invalid record '{}': {}", record.path, e.message);
				toDelete.push(record);
			}
		}

		if (!allSingleEntries) {
			Exception.assert(
				totalSize <= this.options.maxSize,
				"The total record size is too large: {} vs {}",
				totalSize,
				this.options.maxSize,
			);
		}

		// Delete invalid records.
		for (const record of toDelete) {
			await this._delete(storage, record);
		}

		return [tick, remoteTick];
	}

	/// Ensure that everything is in order and reset the tick count.
	async sanitize(callback) {
		for (const storage of Object.values(this.storages)) {
			await storage.lock.acquire(async () => {
				await this._sanitizeStorage(storage, callback);
			});
		}
	}
}
