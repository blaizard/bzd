import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import makeFromConfig from "#bzd/nodejs/db/storage/make_from_config.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";
import { timestampUs } from "#bzd/nodejs/utils/timestamp.js";
import mime from "mime";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "files");

/// Value representing a file to be stored.
export class File {
	constructor({ stream, mime = null, filename = null, metadata = {} }) {
		this.stream = stream;
		this.mime = mime;
		this.filename = filename;
		this.metadata = metadata;
	}
}

/// Store values in an object storage and return their pointer.
export class FileStore {
	constructor(options = {}) {
		Exception.assert(options.storage, "'nodes.filestore.storage' must be set.");
		this.options = options;
		this.storage = null;
	}

	/// Initialize the storage.
	async initialize() {
		this.storage = await makeFromConfig(Object.assign({}, this.options.storage, { write: true }));
	}

	/// Check if a value is a file pointer.
	static isFile(value) {
		return value !== null && typeof value === "object" && typeof value["$file"] === "string";
	}

	/// Get the size of a value that can be stored as is, 0 otherwise.
	static sizeOf(value) {
		if (typeof value === "string") {
			return Buffer.byteLength(value);
		}
		if (value instanceof Uint8Array) {
			return value.byteLength;
		}
		return 0;
	}

	/// Store files and large values, replacing them with their pointers.
	async process(fragments, { uid, timestamp }) {
		const limit = this.options.sizeToFileStore ?? 10 * 1024;
		const fileTimestamp = timestamp ?? timestampUs();
		for (const fragment of fragments) {
			if (fragment[1] instanceof File) {
				fragment[1] = await this.store({
					uid,
					key: fragment[0],
					stream: fragment[1].stream,
					mime: fragment[1].mime,
					metadata: fragment[1].metadata,
					timestamp: fileTimestamp,
				});
			} else if (limit && FileStore.sizeOf(fragment[1]) > limit) {
				fragment[1] = await this.store({
					uid,
					key: fragment[0],
					stream: fragment[1],
					mime: typeof fragment[1] === "string" ? "text/plain" : "application/octet-stream",
					timestamp: fileTimestamp,
				});
			}
		}
	}

	/// Store a value and return its pointer.
	///
	/// \param uid The uid of the node.
	/// \param key The key of the node.
	/// \param stream The content of the file.
	/// \param mimeType The mime type of the file.
	/// \param metadata Additional metadata to be added to the pointer.
	/// \param timestamp The timestamp in microseconds to be used in the filename.
	async store({ uid, key, stream, mime: mimeType = null, metadata = {}, timestamp }) {
		const extension = mime.extension(mimeType || "application/octet-stream") || "bin";
		const date = new Date(Number(timestamp) / 1000);
		const directory = [
			String(date.getUTCFullYear()).padStart(4, "0"),
			String(date.getUTCMonth() + 1).padStart(2, "0"),
			String(date.getUTCDate()).padStart(2, "0"),
			uid,
			...key,
		];
		let filename = String(timestamp) + "." + extension;
		let counter = 0;
		while (await this.storage.is([...directory, filename])) {
			filename = String(timestamp) + "-" + ++counter + "." + extension;
		}

		const path = [...directory, filename];
		if (typeof stream?.pipe === "function") {
			await this.storage.write(path, stream, /*mkdir*/ true);
		} else {
			await this.storage.writeFromChunk(path, stream, /*mkdir*/ true);
		}

		return {
			...metadata,
			$file: Utils.keyToPath(path).slice(1),
		};
	}

	async read(pathList) {
		return await this.storage.read(pathList);
	}
}
