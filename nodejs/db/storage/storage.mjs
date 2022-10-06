import Fs from "fs";

import ExceptionFactory from "../../core/exception.mjs";
import { fromChunk } from "../../core/stream.mjs";
import { AsyncInitialize } from "../utils.mjs";

const Exception = ExceptionFactory("db", "storage");

/// File storage module.
/// Path are addressed by a path list, as some implementation allow slashes as a name.
export default class Storage extends AsyncInitialize {
	constructor() {
		super();
	}

	/// Tell whether a key exists or not
	async is(path, name) {
		return this._isImpl(Array.isArray(path) ? path : [path], name);
	}

	/// List all files under this bucket
	async list(path, maxOrPaging = 20, includeMetadata = false) {
		return this._listImpl(Array.isArray(path) ? path : [path], maxOrPaging, includeMetadata);
	}

	/// Return a file read stream from a specific key
	async read(path, name) {
		return this._readImpl(Array.isArray(path) ? path : [path], name);
	}

	/// Store a file to a specific key
	async writeFromFile(path, name, inputFilePath) {
		Exception.assert(typeof inputFilePath == "string", "Path must be a string, {:j}", inputFilePath);
		const readStream = Fs.createReadStream(inputFilePath);
		return this._writeImpl(Array.isArray(path) ? path : [path], name, readStream);
	}

	/// Store a file to a specific key
	async writeFromChunk(path, name, data) {
		const readStream = fromChunk(data);
		return this._writeImpl(Array.isArray(path) ? path : [path], name, readStream);
	}

	/// Store a file to a specific key
	async write(path, name, readStream) {
		return this._writeImpl(Array.isArray(path) ? path : [path], name, readStream);
	}

	/// Delete a file from a bucket
	async delete(path, name) {
		return this._deleteImpl(Array.isArray(path) ? path : [path], name);
	}
}
