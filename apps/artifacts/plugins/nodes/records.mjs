import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import Lock from "#bzd/nodejs/core/lock.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// Construct on-disk iterable records.
///
/// The format of each entry is as follow:
/// [tick, payload]
export default class Record {
	constructor(options = {}) {
		this.options = Object.assign(
			{
				path: null,
				fs: FileSystem,
				recordMaxSize: 1024 * 1024,
				maxSize: 20 * 1024 * 1024,
			},
			options,
		);
		Exception.assert(this.options.path !== null, "'path' must be set.");
		this.storages = {};
		this.makeStorage(Record.defaultStorageName);
		// A monotonic counter that gets increased for each new payload.
		this.tick = 0;

		Exception.assert(this.options.recordMaxSize < this.options.maxSize);
	}

	/// Create a new record storage.
	makeStorage(name) {
		Exception.assert(!(name in this.storages), "Storage named '{}' already exists.", name);
		this.storages[name] = {
			path: this.options.path + "/" + name,
			lock: new Lock(),
			records: null,
		};
	}

	/// The current version of the records, any records with a different version
	/// will be discarded.
	static get version() {
		return 1;
	}

	/// The name of the default storage.
	static get defaultStorageName() {
		return "main";
	}

	async init(callback) {
		// Initialize all storages.
		for (const storage of Object.values(this.storages)) {
			await this.options.fs.mkdir(storage.path);

			// Initialize the files map.
			const files = await this.options.fs.readdir(storage.path);
			storage.records = [];
			for (const name of files) {
				const path = storage.path + "/" + name;
				const maybeTick = Record.getTickFromPath(path);
				// Invalid file pattern, try to delete the file.
				if (maybeTick === null) {
					Log.error("Invalid record, doesn't match the file pattern: '{}', deleting.", path);
					await this.options.fs.unlink(path);
					continue;
				}
				await this._addRecordEntry(storage, maybeTick, path);
			}
		}

		this.tick = await this.sanitize(callback);
	}

	/// Get the tick from a path.
	static getTickFromPath(path) {
		const filePattern = new RegExp("^(?:.*/)?([0-9]+).rec$");
		const match = filePattern.exec(path);
		return match === null ? null : parseInt(match[1]);
	}

	/// Get payload of a record into a list.
	static payloadToList(payload) {
		const data = JSON.parse(payload + "[]]}");
		Exception.assert(
			data.version == Record.version,
			"Record is from an incompatible version: {} vs {}.",
			data.version,
			Record.version,
		);
		return data.records.slice(0, -1);
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

		return entry;
	}

	_getLastEntryForReading(storage) {
		return storage.records[storage.records.length - 1] || null;
	}

	/// Add new payload to the record list.
	async _getLastEntryForWriting(storage, tick, payloadSize) {
		Exception.assert(storage.records !== null, "Used before initialization.");
		let entry = this._getLastEntryForReading(storage);

		// Check if the entry has still enough space.
		if (entry && entry.size + payloadSize > this.options.recordMaxSize) {
			entry = null;
		}

		// There is no available entry, create a new one.
		if (entry === null) {
			entry = await this._addRecordEntry(storage, tick);
		}

		return entry;
	}

	/// Write new records.
	async write(payload, storageName = Record.defaultStorageName) {
		const serialized = JSON.stringify(payload);
		Exception.assert(storageName in this.storages, "Invalid storage requested: '{}'.", storageName);
		const storage = this.storages[storageName];

		// Get the record and write to it atomically.
		await storage.lock.acquire(async () => {
			const tick = ++this.tick;
			const content = "[" + tick + "," + serialized + "," + serialized.length + "],\n";
			const entry = await this._getLastEntryForWriting(storage, tick, content.length);
			await this.options.fs.appendFile(entry.path, content);
			entry.size += content.length;
		});
	}

	/// Read a record from a tick to its end.
	///
	/// \param tick Initial tick.
	///
	/// \return A tuple, containing the list of payload previously stored, and the next tick.
	///         In case there are no new payloads, null is returned.
	async _read(storage, tick) {
		Exception.assert(storage.records !== null, "Used before initialization.");

		/// If the tick is null, return null, this is to ease looping over _read.
		if (tick === null) {
			return null;
		}

		const maybeEntries = await storage.lock.acquire(async () => {
			// Look for the entry.
			let index = storage.records.findLastIndex((entry) => entry.tick <= tick);
			if (index === -1) {
				index = 0;
			}

			for (; index < storage.records.length; ++index) {
				const entry = storage.records[index];

				// Read the file content.
				const content = await this.options.fs.readFile(entry.path);
				const payloads = Record.payloadToList(content);
				const indexPayload = payloads.findIndex((payloadEntry) => payloadEntry[0] >= tick);

				if (indexPayload !== -1) {
					return payloads.slice(indexPayload);
				}

				// No match, we are either at the end of the records or we need to look for the next record.
			}

			return null;
		});

		if (maybeEntries === null) {
			return null;
		}

		Exception.assert(maybeEntries.length > 0, "The output cannot be empty as we search in it just before.");
		return maybeEntries;
	}

	async *read(tick = 0) {
		let result = null;
		// TODO: loop through all storages.
		const storage = this.storages[Record.defaultStorageName];
		while ((result = await this._read(storage, tick))) {
			let payload = null;
			for (payload of result) {
				yield payload;
			}
			tick = payload[0] + 1;
		}
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
	async _sanitize(storage, callback) {
		let tick = null;
		let totalSize = 0;
		let allSingleEntries = true;
		let toDelete = [];
		for (const record of storage.records) {
			const maybeTick = Record.getTickFromPath(record.path);
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
				const content = await this.options.fs.readFile(record.path);
				Exception.assert(
					content.length == record.size,
					"There is a mismatch between the payload size and the size recorded: {} vs {}.",
					content.length,
					record.size,
				);
				const entries = Record.payloadToList(content);

				for (const [t, entry, _] of entries) {
					Exception.assert(t > tick, "Ticks are increasing, expected > {} but got {}", tick, t);
					tick = t;
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
				Log.warning("Ignoring invalid record '{}': {}", record.path, e.message);
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

		return tick;
	}

	/// Ensure that everything is in order and reset the tick count.
	async sanitize(callback) {
		// TODO: loop through all storages.
		const storage = this.storages[Record.defaultStorageName];
		return await storage.lock.acquire(async () => {
			return await this._sanitize(storage, callback);
		});
	}
}
