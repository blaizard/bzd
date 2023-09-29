import LogFactory from "../../core/log.mjs";
import { makeUid } from "../../utils/uid.mjs";
import { CollectionPaging } from "../utils.mjs";

import KeyValueStore from "./key_value_store.mjs";

const Log = LogFactory("db", "kvs", "memory");

/// Key value store for testing purposes, that presists in memory.
export default class KeyValueStoreMemory extends KeyValueStore {
	constructor(name, options) {
		super();

		this.name = name;
		this.buckets = {};
		this.options = options;

		// Initialize ansynchronously the rest of the data
		Log.info("Using memory key value store DB with name '{}'.", this.name);
	}

	/// Initialization of the class
	async _initialize() {}

	async _setImpl(bucket, key, value) {
		const uid = key === null ? makeUid() : key;
		if (!(bucket in this.buckets)) {
			this.buckets[bucket] = {};
		}
		this.buckets[bucket][key] = value;
		return uid;
	}

	async _getImpl(bucket, key, defaultValue) {
		if (!(bucket in this.buckets)) {
			return defaultValue;
		}
		return key in this.buckets[bucket] ? this.buckets[bucket][key] : defaultValue;
	}

	async _countImpl(bucket) {
		if (!(bucket in this.buckets)) {
			return 0;
		}
		return Object.keys(this.buckets[bucket]).length;
	}

	async _updateImpl(bucket, key, modifier, defaultValue, maxConflicts) {
		const value = modifier(this._getImpl(bucket, key, defaultValue));
		this._setImpl(bucket, key, value);
		return true;
	}

	/// List all key/value pairs from this bucket.
	/// \param bucket The bucket to be used.
	/// \param maxOrPaging Paging information.
	async _listImpl(bucket, maxOrPaging) {
		if (!(bucket in this.buckets)) {
			return await CollectionPaging.makeFromObject({}, maxOrPaging);
		}
		return await CollectionPaging.makeFromObject(this.buckets[bucket], maxOrPaging);
	}

	/// List all key/value pairs from this bucket which subkey matches the value (or any of the values).
	/// \param bucket The bucket to be used.
	/// \param subKey The subkey for the match.
	/// \param value The value of values (if a list) to match.
	/// \param maxOrPaging Paging information.
	/// \return An object containing the data and the information about paging and how to get the rest of the data.
	async _listMatchImpl(bucket, subKey, value, maxOrPaging) {
		if (!(bucket in this.buckets)) {
			return await CollectionPaging.makeFromObject({}, maxOrPaging);
		}
		const data = this.buckets[bucket];
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
		if (bucket in this.buckets) {
			delete this.buckets[bucket][key];
		}
	}
}
