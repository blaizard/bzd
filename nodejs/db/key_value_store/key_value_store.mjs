import { AsyncInitialize } from "../utils.mjs";
import ExceptionFactory from "../../core/exception.mjs";

const Exception = ExceptionFactory("db", "kvs");

/**
 * Key value store for low demanding application, that presists on the local disk.
 */
export default class KeyValueStore extends AsyncInitialize {

	constructor() {
		super();
	}

	/**
	 * Set a key/value pair.
	 * 
	 * If key is set to null, a unique key is automatically generated.
	 */
	async set(/*bucket, key, value*/) {
		Exception.unreachable("'set' must be implemented.");
	}

	/**
	 * Retrieve a value from a given key, or return the default value if it does not exists.
	 */
	async get(/*bucket, key, defaultValue = undefined*/) {
		Exception.unreachable("'get' must be implemented.");
	}

	/**
	 * List all key/value pairs from this bucket.
	 * \param bucket The bucket to be used.
	 * \param maxOrPaging Paging information.
	 */
	async list(/*bucket, maxOrPaging = 10*/) {
		Exception.unreachable("'list' must be implemented.");
	}

	/**
	 * List all key/value pairs from this bucket which subkey matches the value (or any of the values). 
	 * \param bucket The bucket to be used.
	 * \param subKey The subkey for the match.
	 * \param value The value of values (if a list) to match.
	 * \param maxOrPaging Paging information.
	 * \return An object contianing the data and the information about paging and how to get the rest of the data.
	 */
	async listMatch(/*bucket, subKey, value, maxOrPaging = 10*/) {
		Exception.unreachable("'listMatch' must be implemented.");
	}

	/**
	 * Delete an existing key/value pair.
	 */
	async delete(/*bucket, key*/) {
		Exception.unreachable("'delete' must be implemented.");
	}
}
