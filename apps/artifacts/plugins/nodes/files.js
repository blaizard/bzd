import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Router from "#bzd/nodejs/core/router.js";
import { fromChunk } from "#bzd/nodejs/core/stream.js";
import StorageDisk from "#bzd/nodejs/db/storage/disk.js";
import { FileNotFoundError } from "#bzd/nodejs/db/storage/storage.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";
import mime from "mime";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "files");

const PREFIX = "@file";

/// Store files on disk and expose them as time-related entries.
///
/// Files are stored under: <root>/<uid>/<key...>/<timestampUs>-<counter>.<ext>
/// The path is referenced in the nodes data as: "@file/<uid>/<key...>/<filename>".
export default class Files {
	constructor(options = {}) {
		Exception.assert(options.path, "'nodes.files.path' must be set.");
		this.options = options;
		this.storage = null;
		this.router = new Router();
		for (const match of options.keys ?? []) {
			this.router.add(match, () => {});
		}
	}

	/// Initialize the storage.
	async initialize() {
		this.storage = await StorageDisk.make(this.options.path, {
			write: true,
			...(this.options.fs ? { fs: this.options.fs } : {}),
		});
	}

	/// Check if a value is a file pointer.
	static isPointer(value) {
		return typeof value === "string" && value.startsWith(PREFIX + "/");
	}

	/// Convert a uid, key and filename into a file pointer.
	static makePointer(uid, key, filename) {
		return PREFIX + Utils.keyToPath([uid, ...key, filename]);
	}

	/// Convert a file pointer into a path list.
	static pointerToPathList(pointer) {
		Exception.assertPrecondition(Files.isPointer(pointer), "Invalid file pointer: {:?}.", pointer);
		return Utils.pathToKey(pointer.slice(PREFIX.length));
	}

	/// Extract the timestamp from a filename, or null if invalid.
	static timestampFromFilename(filename) {
		const timestamp = parseInt(filename, 10);
		return Number.isNaN(timestamp) ? null : timestamp;
	}

	/// Check if a key is backed by the file store.
	matches(key) {
		return Boolean(this.router.match(Utils.keyToPath(key)));
	}

	/// Write a new file and return its pointer.
	///
	/// \param uid The uid of the node.
	/// \param key The key of the node.
	/// \param data The content of the file.
	/// \param mimeType The mime type of the file.
	/// \param timestamp The timestamp in microseconds to be used in the filename.
	async write({ uid, key, data, mime: mimeType, timestamp }) {
		const extension = mime.extension(mimeType) || "bin";
		const directory = [uid, ...key];

		let filename = String(timestamp) + "." + extension;
		let counter = 0;
		while (await this.storage.is([...directory, filename])) {
			filename = String(timestamp) + "-" + ++counter + "." + extension;
		}

		await this.storage.write([...directory, filename], fromChunk(data), /*mkdir*/ true);

		return {
			filename: filename,
			pointer: Files.makePointer(uid, key, filename),
		};
	}

	/// Copy a file from a remote instance if it is not available locally.
	///
	/// \param client The HTTP client factory pointing to the remote volume.
	/// \param pointer The pointer of the file to be copied.
	///
	/// \return true if the file was copied, false if it was already present.
	async syncFrom(client, pointer) {
		const pathList = Files.pointerToPathList(pointer);
		if (await this.storage.is(pathList)) {
			return false;
		}
		const stream = await client.get("/" + pointer, { expect: "stream" });
		await this.storage.write(pathList, stream, /*mkdir*/ true);
		return true;
	}

	async read(pathList) {
		return await this.storage.read(pathList);
	}

	/// List all files of a given key.
	async list(uid, key) {
		try {
			return await this.storage.listAll([uid, ...key]);
		} catch (e) {
			if (e instanceof FileNotFoundError) {
				return [];
			}
			throw e;
		}
	}

	/// Get older entries of a key from the file store.
	async onExternal(uid, key, count, after, before) {
		count = count ?? 1;
		if (count <= 0) {
			return [];
		}
		const filenames = await this.list(uid, key);
		let entries = filenames
			.map((filename) => [Files.timestampFromFilename(filename), filename])
			.filter(([timestamp]) => timestamp !== null);

		if (entries.length === 0) {
			return [];
		}

		entries.sort((a, b) => a[0] - b[0]);
		const toResult = ([timestamp, filename]) => [timestamp, Files.makePointer(uid, key, filename)];

		if (after !== null && before !== null) {
			// Mirror the behavior of the InfluxDB provider: return at most one entry per bucket.
			const period = Math.max(Math.floor((before - after) / count), 1);
			const buckets = new Map();
			for (const entry of entries) {
				const timestamp = entry[0];
				if (timestamp <= after || timestamp >= before) {
					continue;
				}
				const bucket = Math.floor((timestamp - after) / period);
				if (!buckets.has(bucket)) {
					buckets.set(bucket, entry);
				}
			}
			return [...buckets.values()].map(toResult);
		}

		if (after !== null) {
			return entries
				.filter(([timestamp]) => timestamp > after)
				.slice(0, count)
				.map(toResult);
		}

		entries.reverse();
		if (before !== null) {
			entries = entries.filter(([timestamp]) => timestamp < before);
		}
		return entries.slice(0, count).map(toResult);
	}
}
