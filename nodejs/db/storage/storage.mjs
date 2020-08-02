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
	async is(bucket, key) {
		return this._isImpl(bucket, key);
	}

	/**
	 * List all files under this bucket
	 */
	async list(bucket, maxOrPaging = 50, includeMetadata = false) {
		return this._listImpl(bucket, maxOrPaging, includeMetadata);
	}

	/**
	 * Return a file read stream from a specific key
	 */
	async read(bucket, key) {
		return this._readImpl(bucket, key);
	}

	/**
	 * Store a file to a specific key
	 */
	async writeFromFile(bucket, key, path) {
		Exception.assert(typeof path == "string", "Path must be a string, {:j}", path);
		const readStream = Fs.createReadStream(path);
		return this._writeImpl(bucket, key, readStream);
	}

	/**
	 * Store a file to a specific key
	 */
	async writeFromChunk(bucket, key, data) {
		const readStream = fromChunk(data);
		return this._writeImpl(bucket, key, readStream);
	}

	/**
	 * Store a file to a specific key
	 */
	async write(bucket, key, readStream) {
		return this._writeImpl(bucket, key, readStream);
	}

	/**
	 * Delete a file from a bucket
	 */
	async delete(bucket, key) {
		return this._deleteImpl(bucket, key);
	}
}
