import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import KeyValueStore from "./key_value_store.mjs";
import { CollectionPaging } from "../utils.mjs";

import { MongoClient } from "mongodb";

const Exception = ExceptionFactory("db", "kvs", "mongodb");
const Log = LogFactory("db", "kvs", "mongodb");

/// Key value store adapter to elastic search DB
export default class KeyValueStoreMongodb extends KeyValueStore {
	constructor(uri, options) {
		super();

		this.uri = uri;
		this.db = null;
		this.options = Object.assign(
			{
				name: "kvs",
			},
			options,
		);

		Log.info("Using mongodb as a key value store DB with collection '{}'.", this.options.name);
	}

	/// Initialization of the class
	async _initialize() {
		const client = new MongoClient(this.uri);
		await client.connect();
		this.db = client.db(this.options.name);
	}

	async _setImpl(bucket, key, value) {
		const collection = this.db.collection(bucket);
		await collection.updateOne(
			{ key: key },
			{ $set: { key: key, value: value }, $inc: { transaction: 1 } },
			{ upsert: true },
		);
		return key;
	}

	async _getImpl(bucket, key, defaultValue) {
		const collection = this.db.collection(bucket);
		const result = await collection.findOne({ key: key });
		if (result) {
			return result.value;
		}
		return defaultValue;
	}

	async _countImpl(bucket) {
		const collection = this.db.collection(bucket);
		return await collection.count();
	}

	async _updateImpl(bucket, key, modifier, defaultValue, maxConflicts) {
		const collection = this.db.collection(bucket);

		do {
			const result = await collection.findOne({ key: key });

			// There is no entry with this key.
			if (!result) {
				const value = await modifier(defaultValue);
				await collection.createIndex({ key: 1 }, { unique: true });
				try {
					await collection.insertOne({ key: key, transaction: 1, value: value });
					return true;
				} catch (e) {
					// ignore, it means that the key has been inserted in the mean time.
				}
			}
			// It exists, just update.
			else {
				const value = await modifier(result.value);
				const output = await collection.updateOne(
					{ key: key, transaction: result.transaction },
					{ $set: { value: value }, $inc: { transaction: 1 } },
				);
				if (output.matchedCount == 1) {
					return true;
				}
			}
		} while (maxConflicts--);

		return false;
	}

	async _deleteImpl(bucket, key) {
		const collection = this.db.collection(bucket);
		await collection.deleteOne({ key: key });
	}

	async _listImpl(bucket, maxOrPaging, filter = {}) {
		const collection = this.db.collection(bucket);
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);

		const cursor = await collection.find(filter, {
			limit: paging.max + 1, // +1 to calculate the paging.
			skip: paging.page * paging.max,
		});

		try {
			const entries = await cursor.toArray();
			const nextPaging = entries.length > paging.max ? { page: paging.page + 1, max: paging.max } : null;
			const data = entries.slice(0, paging.max).reduce((obj, value) => {
				obj[value.key] = value.value;
				return obj;
			}, {});
			return new CollectionPaging(data, nextPaging);
		} finally {
			await cursor.close();
		}
	}

	async _listMatchImpl(bucket, subKey, value, maxOrPaging) {
		return await this._listImpl(bucket, maxOrPaging, {
			["value." + subKey]: value,
		});
	}
}
