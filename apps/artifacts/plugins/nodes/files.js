import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { fromChunk } from "#bzd/nodejs/core/stream.js";
import StorageDisk from "#bzd/nodejs/db/storage/disk.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";
import { timestampUs } from "#bzd/nodejs/utils/timestamp.js";
import mime from "mime";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "files");

const PREFIX = "@file";

/// Value representing a file to be stored.
export class FileValue {
	constructor({ uid, stream, mime = null, filename = null }) {
		this.uid = uid;
		this.stream = stream;
		this.mime = mime;
		this.filename = filename;
	}
}

/// Store files on disk and expose them as time-related entries.
///
/// Files are stored under: <root>/<uid>/<key...>/<timestampUs>-<counter>.<ext>
/// The path is referenced in the nodes data as: "@file/<uid>/<key...>/<filename>".
export class FileDatabase {
	constructor(options = {}) {
		Exception.assert(options.path, "'nodes.files.path' must be set.");
		this.options = options;
		this.storage = null;
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
		Exception.assertPrecondition(FileDatabase.isPointer(pointer), "Invalid file pointer: {:?}.", pointer);
		return Utils.pathToKey(pointer.slice(PREFIX.length));
	}

	/// Write a new file and return its pointer.
	///
	/// \param uid The uid of the node.
	/// \param key The key of the node.
	/// \param stream The content of the file.
	/// \param mimeType The mime type of the file.
	/// \param timestamp The timestamp in microseconds to be used in the filename.
	async write({ uid, key, stream, mime: mimeType, timestamp }) {
		const extension = mime.extension(mimeType || "application/octet-stream") || "bin";
		const directory = [uid, ...key];
		let storedFilename = String(timestamp) + "." + extension;
		let counter = 0;
		while (await this.storage.is([...directory, storedFilename])) {
			storedFilename = String(timestamp) + "-" + ++counter + "." + extension;
		}

		const data = typeof stream?.pipe === "function" ? stream : fromChunk(stream);
		await this.storage.write([...directory, storedFilename], data, /*mkdir*/ true);

		return {
			filename: storedFilename,
			pointer: FileDatabase.makePointer(uid, key, storedFilename),
		};
	}

	/// Copy a file from a remote instance if it is not available locally.
	///
	/// \param client The HTTP client factory pointing to the remote volume.
	/// \param pointer The pointer of the file to be copied.
	///
	/// \return true if the file was copied, false if it was already present.
	async syncFrom(client, pointer) {
		const pathList = FileDatabase.pointerToPathList(pointer);
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
}

/// Handler that stores file values and replaces them with their pointer.
export class FilesHandler {
	constructor(files) {
		this.files = files;
	}

	async process(range) {
		for (const fragment of range.all()) {
			fragment.value = (
				await this.files.write({
					key: fragment.fullKey,
					timestamp: timestampUs(),
					...fragment.value,
				})
			).pointer;
		}
	}
}
