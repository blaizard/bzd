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
			},
			options,
		);
		this.path = path;
		this.lock = new Lock();
		this.entries = null;
	}

	async init() {
		await this.options.fs.mkdir(this.path);

		// Initialize the files map.
		const files = await this.options.fs.readdir(this.path);
		const filePattern = new RegExp("^([0-9]+).rec$");
		this.entries = [];
		for (const name of files) {
			const match = filePattern.exec(name);
			// Invalid file pattern, try to delete the file.
			if (match === null) {
				Log.error("Invalid record, doesn't match the file pattern: '{}', deleting.", name);
				await this.options.fs.unlink(this.path + "/" + name);
				continue;
			}
			await this._addRecordEntry(parseInt(match[1]), this.path + "/" + name);
		}
	}

	/// Get the timestamp.
	static getTimestamp() {
		return Date.now();
	}

	/// Register a new entry to the records.
	///
	/// \param timestamp The timestamp related to the file to be created.
	/// \param path The path of the existing record if any, null otherwise.
	async _addRecordEntry(timestamp, path = null) {
		if (path === null) {
			path = this.path + "/" + timestamp + ".rec";
			await this.options.fs.touch(path);
		}
		const entry = {
			timestamp: timestamp,
			path: path,
			size: (await this.options.fs.stat(path)).size,
		};
		this.entries.push(entry);
		this.entries.sort((a, b) => a.timestamp - b.timestamp);
		return entry;
	}

	/// Add new payload to the record list.
	async _getEntry(timestamp, payloadSize) {
		Exception.assert(this.entries !== null, "Used before initialization.");

		let entry = null;
		if (this.entries.length > 0) {
			entry = this.entries[this.entries.length - 1];
			// Check if the entry has still enough space.
			if (entry.size + payloadSize > this.options.recordMaxSize) {
				entry = null;
			}
		}

		// There is no available entry, create a new one.
		if (entry === null) {
			entry = await this._addRecordEntry(timestamp);
		}

		return entry;
	}

	/// Write new records.
	async write(payload) {
		const serialized = JSON.stringify(payload);

		// Get the record and write to it atomically.
		await this.lock.acquire(async () => {
			const timestamp = Record.getTimestamp();
			const content = "[" + timestamp + "," + serialized + "],\n";
			const entry = await this._getEntry(timestamp, content.length);
			await this.options.fs.appendFile(entry.path, content);
			entry.size += content.length;
		});
	}

	/// Read records.
	///
	/// \param timestamp Initial timestamp.
	async read(timestamp = 0) {
		Exception.assert(this.entries !== null, "Used before initialization.");

		const maybeResult = await this.lock.acquire(async () => {
			// Look for the entry
			let entry = this.entries[0] || null;
			let nextEntry = null;
			for (nextEntry of this.entries.slice(1)) {
				if (nextEntry.timestamp > timestamp) {
					break;
				}
				entry = nextEntry;
			}
			if (entry && timestamp > entry.timestamp) {
				entry = null;
			}
			if (entry === null) {
				return null;
			}

			// Read the file content.
			const content = await this.options.fs.readFile(entry.path);
			const nextTimestamp = nextEntry && nextEntry !== entry ? nextEntry.timestamp : null;
			return [content, nextTimestamp];
		});

		if (maybeResult === null) {
			return null;
		}

		const [content, nextTimestamp] = maybeResult;
		const payloads = JSON.parse("[" + content + "[]]").slice(0, -1);
		const index = payloads.find((payloadEntry) => payloadEntry[0] >= timestamp);
		return [payloads.slice(index).map(([_, payload]) => payload), nextTimestamp];
	}
}
