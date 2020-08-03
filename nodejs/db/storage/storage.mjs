import Fs from "fs";

import ExceptionFactory from "../../core/exception.mjs";
import { fromChunk } from "../../core/stream.mjs";
import { AsyncInitialize } from "../utils.mjs";

const Exception = ExceptionFactory("db", "storage");

/**
 * File storage module
 */
export default class Storage extends AsyncInitialize {
	constructor() {
		super();
	}

	/**
	 * Tell whether a key exists or not
	 */
	async is(path, name) {
		return this._isImpl(path, name);
	}

	/**
	 * List all files under this bucket
	 */
	async list(path, maxOrPaging = 50, includeMetadata = false) {
		return this._listImpl(path, maxOrPaging, includeMetadata);
	}

	/**
	 * Return a file read stream from a specific key
	 */
	async read(path, name) {
		return this._readImpl(path, name);
	}

	/**
	 * Store a file to a specific key
	 */
	async writeFromFile(path, name, inputFilePath) {
		Exception.assert(typeof inputFilePath == "string", "Path must be a string, {:j}", inputFilePath);
		const readStream = Fs.createReadStream(inputFilePath);
		return this._writeImpl(path, name, readStream);
	}

	/**
	 * Store a file to a specific key
	 */
	async writeFromChunk(path, name, data) {
		const readStream = fromChunk(data);
		return this._writeImpl(path, name, readStream);
	}

	/**
	 * Store a file to a specific key
	 */
	async write(path, name, readStream) {
		return this._writeImpl(path, name, readStream);
	}

	/**
	 * Delete a file from a bucket
	 */
	async delete(path, name) {
		return this._deleteImpl(path, name);
	}
}
