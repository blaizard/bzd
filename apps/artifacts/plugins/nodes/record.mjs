import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import Lock from "#bzd/nodejs/core/lock.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// Construct on-disk iterable records.
///
/// The format of each entry is as follow:
/// [timestamp, payload]
export default class Record {
	constructor(path, options = {}) {
		this.options = Object.assign(
			{
				fs: FileSystem,
				recordMaxSize: 1024 * 1024,
				maxSize: 20 * 1024 * 1024,
			},
			options,
		);
		this.path = path;
		this.lock = new Lock();
		this.records = null;

		Exception.assert(this.options.recordMaxSize < this.options.maxSize);
	}

	async init() {
		await this.options.fs.mkdir(this.path);

		// Initialize the files map.
		const files = await this.options.fs.readdir(this.path);
		this.records = [];
		for (const name of files) {
			const path = this.path + "/" + name;
			const maybeTimestamp = Record.getTimestampFromPath(path);
			// Invalid file pattern, try to delete the file.
			if (maybeTimestamp === null) {
				Log.error("Invalid record, doesn't match the file pattern: '{}', deleting.", path);
				await this.options.fs.unlink(path);
				continue;
			}
			await this._addRecordEntry(maybeTimestamp, path);
		}

		await this.sanityCheck(/*includePayload*/ false);
	}

	/// Get the timestamp from a path.
	static getTimestampFromPath(path) {
		const filePattern = new RegExp("^(?:.*/)?([0-9]+).rec$");
		const match = filePattern.exec(path);
		return match === null ? null : parseInt(match[1]);
	}

	/// Get the timestamp.
	static getTimestamp() {
		return Date.now();
	}

	/// Get payload of a record into a list.
	static payloadToList(payload) {
		return JSON.parse("[" + payload + "[]]").slice(0, -1);
	}

	/// Register a new entry to the records.
	///
	/// \param timestamp The timestamp related to the file to be created.
	/// \param path The path of the existing record if any, null otherwise.
	async _addRecordEntry(timestamp, path = null) {
		// Look for the index that shall be just before the one to insert.
		let index = null;
		while (true) {
			index = this.records.findLastIndex((entry) => entry.timestamp <= timestamp);
			// If the timestamp is the same as an existing one, increase it as it cannot be.
			if (index !== -1 && this.records[index].timestamp == timestamp) {
				++timestamp;
				continue;
			}
			break;
		}

		// Create the file if it does not exists.
		if (path === null) {
			path = this.path + "/" + timestamp + ".rec";
			await this.options.fs.touch(path);
		}

		// Create and insert the new entry.
		const entry = {
			timestamp: timestamp,
			path: path,
			size: (await this.options.fs.stat(path)).size,
			next: this.records[index + 1] ? this.records[index + 1].timestamp : null,
		};
		this.records.splice(index + 1, 0, entry);
		if (this.records[index]) {
			this.records[index].next = timestamp;
		}

		return [timestamp, entry];
	}

	/// Add new payload to the record list.
	async _getLastEntry(payloadSize) {
		Exception.assert(this.records !== null, "Used before initialization.");
		let timestamp = Record.getTimestamp();

		let entry = null;
		if (this.records.length > 0) {
			entry = this.records[this.records.length - 1];
			// Check if the entry has still enough space.
			if (entry.size + payloadSize + 20 > this.options.recordMaxSize) {
				entry = null;
			}
		}

		// There is no available entry, create a new one.
		if (entry === null) {
			// +1 to the timestamp to ensure that a new record is always greater than the last
			// entry of the previous record.
			[timestamp, entry] = await this._addRecordEntry(timestamp + 1);
		}

		// Make sure that the timestamp is at least greater than the entry timestamp.
		// This can be untrue due to the +1 above.
		timestamp = Math.max(timestamp, entry.timestamp);

		return [timestamp, entry];
	}

	/// Write new records.
	async write(payload) {
		const serialized = JSON.stringify(payload);

		// Get the record and write to it atomically.
		await this.lock.acquire(async () => {
			const [timestamp, entry] = await this._getLastEntry(serialized.length);
			const content = "[" + timestamp + "," + serialized + "],\n";
			await this.options.fs.appendFile(entry.path, content);
			entry.size += content.length;
		});
	}

	/// Read a record from a timestamp to its end.
	///
	/// \param timestamp Initial timestamp.
	/// \param depth Number of recursive calls.
	///
	/// \return A tuple, containing the list of payload previously stored, and the next timestamp.
	///         In case there are no new payloads, null is returned.
	async _read(timestamp, depth = 0) {
		Exception.assert(this.records !== null, "Used before initialization.");
		Exception.assert(depth < 3, "Too many recursive calls, something went wrong, timestamp: {}", timestamp);

		/// If the timestamp is null, return null, this is to ease looping over _read.
		if (timestamp === null) {
			return null;
		}

		const maybeResult = await this.lock.acquire(async () => {
			// Look for the entry.
			let index = this.records.findLastIndex((entry) => entry.timestamp <= timestamp);
			if (index === -1) {
				index = 0;
			}
			const entry = this.records[index] || null;
			if (entry === null) {
				return null;
			}
			// Read the file content.
			const content = await this.options.fs.readFile(entry.path);
			return [content, entry.next];
		});

		if (maybeResult === null) {
			return null;
		}

		const [content, nextTimestamp] = maybeResult;
		const payloads = Record.payloadToList(content);
		const index = payloads.findIndex((payloadEntry) => payloadEntry[0] >= timestamp);

		// No match, it means that all entries are older than what is requested.
		// In that case, go to the next record.
		if (index === -1) {
			if (nextTimestamp !== null) {
				return await this._read(nextTimestamp, depth + 1);
			}
			return null;
		}

		const entries = payloads.slice(index).map(([_, payload]) => payload);
		Exception.assert(entries.length > 0, "The output cannot be empty as we search in it just before.");
		return [payloads.slice(index).map(([_, payload]) => payload), nextTimestamp];
	}

	async *read(timestamp = 0) {
		let result = null;
		while ((result = await this._read(timestamp))) {
			const [payloads, nextTimestamp] = result;
			yield payloads;
			timestamp = nextTimestamp;
		}
	}

	/// Ensure that everything is in order.
	async sanityCheck(includePayload) {
		let timestamp = -2;
		let timestampRecord = -1;
		let totalSize = 0;
		let lastRecord = null;
		for (const record of this.records) {
			const maybeTimestamp = Record.getTimestampFromPath(record.path);
			Exception.assert(maybeTimestamp !== null, "Timestamp cannot be extracted from path '{}'.", record.path);
			Exception.assert(
				record.timestamp == maybeTimestamp,
				"Path '{}' and timestamp '{}' do not match.",
				record.path,
				record.timestamp,
			);
			Exception.assert(
				timestampRecord < record.timestamp,
				"Timestamp of the record must be greater than the previous one.",
			);
			Exception.assert(
				record.timestamp > timestamp,
				"Timestamp of the record must be greater than the last entry: {} vs {}, records {} -> {}",
				record.timestamp,
				timestamp,
				JSON.stringify(lastRecord),
				JSON.stringify(record),
			);
			Exception.assert(
				record.size <= this.options.recordMaxSize,
				"The record size is too large: {} vs {}",
				record.size,
				this.options.recordMaxSize,
			);

			timestampRecord = record.timestamp;
			timestamp = record.timestamp;
			totalSize += record.size;
			lastRecord = record;

			if (includePayload) {
				const content = await this.options.fs.readFile(record.path);
				Exception.assert(
					content.length == record.size,
					"There is a mismatch between the payload size and the size recorded: {} vs {}.",
					content.length,
					record.size,
				);
				const entries = Record.payloadToList(content);
				for (const [t, entry] of entries) {
					Exception.assert(timestamp <= t, "The records are unordered: {}", entries);
					timestamp = t;
				}
			}
		}

		Exception.assert(
			totalSize <= this.options.maxSize,
			"The total record size is too large: {} vs {}",
			totalSize,
			this.options.maxSize,
		);
	}
}
