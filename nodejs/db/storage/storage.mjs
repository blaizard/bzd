import ExceptionFactory from "../../core/exception.mjs";
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
	async is(/*bucket, key*/) {
		Exception.unreachable("'is' must be implemented.");
	}

	/**
	 * List all files under this bucket
	 */
	async list(/*bucket*/) {
		Exception.unreachable("'list' must be implemented.");
	}

	/**
	 * Return a file read stream from a specific key
	 */
	async read(/*bucket, key*/) {
		Exception.unreachable("'read' must be implemented.");
	}

	/**
	 * Store a file to a specific key
	 */
	async write(/*bucket, key, data*/) {
		Exception.unreachable("'write' must be implemented.");
	}

	/**
	 * Delete a file from a bucket
	 */
	async delete(/*bucket, key*/) {
		Exception.unreachable("'delete' must be implemented.");
	}
}
