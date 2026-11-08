import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { fromChunk } from "#bzd/nodejs/core/stream.js";
import makeFromConfig from "#bzd/nodejs/db/storage/make_from_config.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";
import { timestampUs } from "#bzd/nodejs/utils/timestamp.js";
import mime from "mime";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "files");

/// Value representing a file to be stored.
export class File {
	constructor({ stream, mime = null, filename = null }) {
		this.stream = stream;
		this.mime = mime;
		this.filename = filename;
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

	/// Estimate the serialized size of a value.
	static sizeOf(value) {
		if (value === undefined) {
			return 0;
		}
		if (typeof value === "string") {
			return Buffer.byteLength(value);
		}
		if (ArrayBuffer.isView(value)) {
			return value.byteLength;
		}
		return Buffer.byteLength(JSON.stringify(value));
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
					timestamp: fileTimestamp,
				});
			} else if (limit && FileStore.sizeOf(fragment[1]) > limit) {
				fragment[1] = await this.store({
					uid,
					key: fragment[0],
					value: fragment[1],
					timestamp: fileTimestamp,
				});
			}
		}
	}

	/// Store a value and return its pointer.
	///
	/// \param uid The uid of the node.
	/// \param key The key of the node.
	/// \param stream The content of the file, if any.
	/// \param value The value to be serialized, if any.
	/// \param mimeType The mime type of the file.
	/// \param timestamp The timestamp in microseconds to be used in the filename.
	async store({ uid, key, stream = null, value = null, mime: mimeType = null, timestamp }) {
		if (value !== null) {
			stream = JSON.stringify(value);
			mimeType = "application/json";
		}

		const extension = mime.extension(mimeType || "application/octet-stream") || "bin";
		const directory = [uid, ...key];
		let filename = String(timestamp) + "." + extension;
		let counter = 0;
		while (await this.storage.is([...directory, filename])) {
			filename = String(timestamp) + "-" + ++counter + "." + extension;
		}

		const data = typeof stream?.pipe === "function" ? stream : fromChunk(stream);
		await this.storage.write([...directory, filename], data, /*mkdir*/ true);

		return {
			$file: Utils.keyToPath([uid, ...key, filename]).slice(1),
		};
	}

	async read(pathList) {
		return await this.storage.read(pathList);
	}
}
