import Path from "path";

import Cache from "../../core/cache.mjs";
import FileSystem from "../../core/filesystem.mjs";
import LogFactory from "../../core/log.mjs";
import PersistenceDisk from "../../core/persistence/disk.mjs";
import { makeUid } from "../../utils/uid.mjs";
import { CollectionPaging } from "../utils.mjs";

import KeyValueStore from "./key_value_store.mjs";

const Log = LogFactory("db", "kvs", "disk");

/// Key value store for low demanding application, that persists on the local disk.
export default class KeyValueStoreDisk extends KeyValueStore {
	constructor(path, options) {
		super();

		this.options = Object.assign(
			{
				// Bucket specific options
				buckets: {},
			},
			options,
		);
		// The path where to store the database
		this.path = path;
		// Contains all open persistencies (aka buckets)
		this.persistences = {};

		this.cache = null;

		// Initialize ansynchronously the rest of the data
		Log.info("Using disk key value store DB at '{}'.", this.path);
	}

	/// Initialization of the class
	async _initialize() {
		// Create the directory if it does not exists
		await FileSystem.mkdir(this.path);
		this.cache = new Cache();
	}

	/// Return the persistence associated with a specific bucket and, if needed, create it and load it.
	async _getPersistence(bucket) {
		if (!this.cache.isCollection(bucket)) {
			// Register this bucket in the cache
			this.cache.register(bucket, async () => {
				// Read bucket specific options
				const optionsBucket = Object.assign(
					{
						/**
						 * \brief Perform a savepoint every X seconds
						 */
						savepointIntervalS: 5 * 60,
					},
					bucket in this.options.buckets ? this.options.buckets[bucket] : {},
				);

				// Load the persistence
				const options = {
					initialize: true,
					savepointTask: {
						namespace: "kvs",
						name: bucket,
						intervalMs: optionsBucket.savepointIntervalS * 1000,
					},
				};

				let persistence = await PersistenceDisk.make(Path.join(this.path, bucket), options);
				// Preload the data in order to get accurate attribute size
				await persistence.get();
				return persistence;
			});
		}

		let persistence = await this.cache.get(bucket);
		return persistence;
	}

	/// This function waits until the key value store database is ready
	async waitReady() {
		return this.event.waitUntil("ready");
	}

	async _setImpl(bucket, key, value) {
		let persistence = await this._getPersistence(bucket);
		const uid = key === null ? makeUid() : key;
		await persistence.write("set", uid, value);
		return uid;
	}

	async _getImpl(bucket, key, defaultValue) {
		let persistence = await this._getPersistence(bucket);
		const data = await persistence.get();
		return key in data ? data[key] : defaultValue;
	}

	async _countImpl(bucket) {
		let persistence = await this._getPersistence(bucket);
		const data = await persistence.get();
		return Object.keys(data).length;
	}

	async _updateImpl(bucket, key, modifier, defaultValue, maxConflicts) {
		let persistence = await this._getPersistence(bucket);

		let isSuccess = false;
		do {
			const version = persistence.getVersion();
			const data = await persistence.get();
			const modifiedValue = await modifier(key in data ? data[key] : defaultValue);
			isSuccess = await persistence.writeCompare(version, "set", key, modifiedValue);
		} while (!isSuccess && maxConflicts--);

		return maxConflicts > 0;
	}

	/// List all key/value pairs from this bucket.
	/// \param bucket The bucket to be used.
	/// \param maxOrPaging Paging information.
	async _listImpl(bucket, maxOrPaging) {
		let persistence = await this._getPersistence(bucket);
		const data = await persistence.get();
		return await CollectionPaging.makeFromObject(data, maxOrPaging);
	}

	/// List all key/value pairs from this bucket which subkey matches the value (or any of the values).
	/// \param bucket The bucket to be used.
	/// \param subKey The subkey for the match.
	/// \param value The value of values (if a list) to match.
	/// \param maxOrPaging Paging information.
	/// \return An object containing the data and the information about paging and how to get the rest of the data.
	async _listMatchImpl(bucket, subKey, value, maxOrPaging) {
		let persistence = await this._getPersistence(bucket);
		const data = await persistence.get();
		const valueList = Array.isArray(value) ? value : [value];
		const filteredData = Object.keys(data)
			.filter((name) => {
				const entry = data[name];
				return subKey in entry && valueList.includes(entry[subKey]);
			})
			.reduce((obj, name) => {
				obj[name] = data[name];
				return obj;
			}, {});
		return await CollectionPaging.makeFromObject(filteredData, maxOrPaging);
	}

	async _deleteImpl(bucket, key) {
		let persistence = await this._getPersistence(bucket);
		await persistence.write("delete", key);
	}
}
