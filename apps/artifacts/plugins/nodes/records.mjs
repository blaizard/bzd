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
		this.lock = new Lock();
		this.records = null;
		// A monotonic counter that gets increased for each new payload.
		this.tick = 0;

		Exception.assert(this.options.recordMaxSize < this.options.maxSize);
	}

	/// The current version of the records, any records with a different version
	/// will be discarded.
	static get version() {
		return 1;
	}

	async init(callback) {
		await this.options.fs.mkdir(this.options.path);

		// Initialize the files map.
		const files = await this.options.fs.readdir(this.options.path);
		this.records = [];
		for (const name of files) {
			const path = this.options.path + "/" + name;
			const maybeTick = Record.getTickFromPath(path);
			// Invalid file pattern, try to delete the file.
			if (maybeTick === null) {
				Log.error("Invalid record, doesn't match the file pattern: '{}', deleting.", path);
				await this.options.fs.unlink(path);
				continue;
			}
			await this._addRecordEntry(maybeTick, path);
		}

		await this.sanitize(callback);
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
	/// \param tick The tick related to the file to be created.
	/// \param path The path of the existing record if any, null otherwise.
	async _addRecordEntry(tick, path = null) {
		// Look for the index that shall be just before the one to insert.
		const index = this.records.findLastIndex((entry) => entry.tick <= tick);
		Exception.assert(index === -1 || this.records[index].tick < tick, "2 ticks cannot have the same value.");

		// Create the file if it does not exists.
		if (path === null) {
			path = this.options.path + "/" + tick + ".rec";
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
		this.records.splice(index + 1, 0, entry);

		// Cleanup previous records if needed.
		let size = this.records.reduce((total, record) => record.size + total, 0);
		while (this.records.length && size > this.options.maxSize - this.options.recordMaxSize) {
			const entry = this.records.shift();
			await this.options.fs.unlink(entry.path);
			size -= entry.size;
		}

		return entry;
	}

	_getLastEntryForReading() {
		return this.records[this.records.length - 1] || null;
	}

	/// Add new payload to the record list.
	async _getLastEntryForWriting(tick, payloadSize) {
		Exception.assert(this.records !== null, "Used before initialization.");
		let entry = this._getLastEntryForReading();

		// Check if the entry has still enough space.
		if (entry && entry.size + payloadSize > this.options.recordMaxSize) {
			entry = null;
		}

		// There is no available entry, create a new one.
		if (entry === null) {
			entry = await this._addRecordEntry(tick);
		}

		return entry;
	}

	/// Write new records.
	async write(payload) {
		const serialized = JSON.stringify(payload);

		// Get the record and write to it atomically.
		await this.lock.acquire(async () => {
			const tick = ++this.tick;
			const content = "[" + tick + "," + serialized + "," + serialized.length + "],\n";
			const entry = await this._getLastEntryForWriting(tick, content.length);
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
	async _read(tick) {
		Exception.assert(this.records !== null, "Used before initialization.");

		/// If the tick is null, return null, this is to ease looping over _read.
		if (tick === null) {
			return null;
		}

		const maybeEntries = await this.lock.acquire(async () => {
			// Look for the entry.
			let index = this.records.findLastIndex((entry) => entry.tick <= tick);
			if (index === -1) {
				index = 0;
			}

			for (; index < this.records.length; ++index) {
				const entry = this.records[index];

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
		while ((result = await this._read(tick))) {
			let payload = null;
			for (payload of result) {
				yield payload;
			}
			tick = payload[0] + 1;
		}
	}

	async _delete(record) {
		for (let index = 0; index < this.records.length; ++index) {
			if (this.records[index].path == record.path) {
				Log.info("Deleting record '{}'.", record.path);
				this.records.splice(index, 1);
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
	async _sanitize(callback) {
		this.tick = null;
		let totalSize = 0;
		let allSingleEntries = true;
		let toDelete = [];
		for (const record of this.records) {
			const maybeTick = Record.getTickFromPath(record.path);
			Exception.assert(maybeTick !== null, "Tick cannot be extracted from path '{}'.", record.path);
			Exception.assert(record.tick == maybeTick, "Path '{}' and tick '{}' do not match.", record.path, record.tick);
			if (this.tick !== null) {
				Exception.assert(
					record.tick > this.tick,
					"Tick of the next record must greater: expected > {} but got {}",
					this.tick,
					record.tick,
				);
			}

			this.tick = record.tick - 1;

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
					Exception.assert(t > this.tick, "Ticks are increasing, expected > {} but got {}", this.tick, t);
					this.tick = t;
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
			await this._delete(record);
		}
	}

	/// Ensure that everything is in order and reset the tick count.
	async sanitize(callback) {
		await this.lock.acquire(async () => {
			await this._sanitize(callback);
		});
	}
}
