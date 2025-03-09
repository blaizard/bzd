import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import Lock from "#bzd/nodejs/core/lock.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Log = LogFactory("apps", "plugin", "nodes");

/// Construct on-disk iterable records.
///
/// The format of each entry is as follow:
/// [timestamp, payload]
export default class Record {
	constructor(path, options = {}) {
		this.options = Object.assign({ fs: FileSystem }, options);
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
			const filePath = this.path + "/" + name;
			const match = filePattern.exec(name);
			// Invalid file pattern, try to delete the file.
			if (match === null) {
				Log.error("Invalid record, doesn't match the file pattern: '{}', deleting.", name);
				await this.options.fs.unlink(filePath);
				continue;
			}
			await this._addRecordEntry(parseInt(match[1]), filePath);
		}
	}

	/// Register a new entry to the records.
	async _addRecordEntry(timestamp, path) {
		this.entries.push({
			timestamp: timestamp,
			path: path,
			size: (await this.options.fs.stat(path)).size,
		});
		this.entries.sort((a, b) => a.timestamp - b.timestamp);
	}

	/// Get the timestamp.
	static getTimestamp() {
		return Date.now();
	}

	/// Find the entry for this record.
	async _getNext(payloadSize) {
		const timestamp = Record.getTimestamp();

		return [timestamp, this.path + "/" + timestamp + ".rec"];
	}

	/// Add new records.
	async add(payload) {
		const serialized = JSON.stringify(payload);

		// Get the record and write to it atomically.
		await this.lock.acquire(async () => {
			const [timestamp, path] = await this._getNext(serialized.length);
			await this.options.fs.appendFile(path, "[" + timestamp + "," + serialized + "],");
		});
	}
}
