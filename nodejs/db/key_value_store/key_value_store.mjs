import { AsyncInitialize } from "../utils.mjs";

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
	async set(bucket, key, value) {
		return this._setImpl(bucket, key, value);
	}

	/**
	 * Retrieve a value from a given key, or return the default value if it does not exists.
	 */
	async get(bucket, key, defaultValue = undefined) {
		return this._getImpl(bucket, key, defaultValue);
	}

	/**
	 * Return the number of entries in this bucket.
	 */
	async count(bucket) {
		return this._countImpl(bucket);
	}

	/**
	 * List all key/value pairs from this bucket.
	 * \param bucket The bucket to be used.
	 * \param maxOrPaging Paging information.
	 */
	async list(bucket, maxOrPaging = 20) {
		return this._listImpl(bucket, maxOrPaging);
	}

	/**
	 * List all key/value pairs from this bucket which subkey matches the value (or any of the values).
	 * \param bucket The bucket to be used.
	 * \param subKey The subkey for the match.
	 * \param value The value of values (if a list) to match.
	 * \param maxOrPaging Paging information.
	 * \return An object contianing the data and the information about paging and how to get the rest of the data.
	 */
	async listMatch(bucket, subKey, value, maxOrPaging = 20) {
		return this._listMatchImpl(bucket, subKey, value, maxOrPaging);
	}

	/**
	 * Delete an existing key/value pair.
	 */
	async delete(bucket, key) {
		return this._deleteImpl(bucket, key);
	}
}
