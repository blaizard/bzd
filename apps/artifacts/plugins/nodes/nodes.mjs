import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Handlers from "#bzd/apps/artifacts/plugins/nodes/handlers/handlers.mjs";
import Optional from "#bzd/nodejs/utils/optional.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// A node is a collection of data.
///
/// Data are denominated by a path, each data can be set at various interval.
/// Data have a validity time that is adjusted automatically.
export class Node {
	constructor(storage, cache, uid, handlers) {
		this.storage = storage;
		this.cache = cache;
		this.uid = uid;
		this.handlers = handlers;
	}

	/// Identify the path of the fragments that are being set.
	///
	/// A path is determined by the full path up to a value or a list.
	///
	/// \return A dictionary of paths and values. Each path is hashed using keyToInternal(...).
	static getAllPathAndValues(fragment, rootKey = []) {
		let paths = {};
		for (const [key, value] of Object.entries(fragment)) {
			if (value && value.constructor == Object) {
				for (const [subKey, subValue] of Object.entries(Node.getAllPathAndValues(value))) {
					paths[KeyMapping.keyToInternal([...rootKey, key, subKey])] = subValue;
				}
			} else {
				paths[KeyMapping.keyToInternal([...rootKey, key])] = value;
			}
		}
		return paths;
	}

	/// Create a copy of the data and call a callback on all values before creating it.
	static mapData(data, callback) {
		if (Array.isArray(data)) {
			return callback(data);
		}
		return Object.fromEntries(
			Object.entries(data).map(([k, v]) => {
				return [k, Node.mapData(v, callback)];
			}),
		);
	}

	/// Insert new data at a given path.
	async insert(key, fragment) {
		const timestamp = Date.now();

		let fragments = Node.getAllPathAndValues(fragment, key);
		fragments = this.handlers.processBeforeInsert(fragments);

		await this.storage.update(
			this.uid,
			async (data) => {
				// Identify the path of the fragments and their values.
				for (const [internal, value] of Object.entries(fragments)) {
					// Prepend the new value and the timestamp to the values array.
					// And ensure there are maximum X elements.
					data[internal] ??= { values: [] };
					const values = data[internal].values;
					values.unshift([timestamp, value]);
					while (values.length > this.handlers.process("history", internal)) {
						values.pop();
					}
				}

				return data;
			},
			{},
		);

		// Invalidate the cache.
		this.cache.setDirty(this.uid);
	}

	/// Get the tree with single values.
	///
	/// \param key The key to locate the data to be returned.
	/// \param isMetadata Whether metadata should be returned or not.
	///        If false, the raw latest value is returned.
	///
	/// \return An optional with a value if success, empty if the key points to an unknown record.
	async get(key, isMetadata = false) {
		const data = await this.cache.get(this.uid);
		const reducedData = key.reduce((r, segment) => {
			if (r === null || !(segment in r)) {
				return null;
			}
			return r[segment];
		}, data);
		if (reducedData === null) {
			return new Optional();
		}
		return new Optional(isMetadata ? reducedData : Node.mapData(reducedData, (v) => v[1]));
	}

	/// Get a specific value at a given path.
	///
	/// \param key The path to which the value is requested.
	/// \param count The number of values to be obtained.
	async getValues(key, count) {
		const internal = KeyMapping.keyToInternal(key);

		const data = await this.storage.get(this.uid, {});
		if (!(internal in data)) {
			return new Optional();
		}
		const value = data[internal];
		return new Optional(value.values.slice(0, count));
	}
}

export class Nodes {
	constructor(storage, handlers) {
		this.storage = storage.getAccessor("data");
		this.handlers = new Handlers(handlers);
		const cache = new Cache({
			garbageCollector: false,
		});
		cache.register("data", async (uid) => {
			// Convert data into a tree.
			const data = await this.storage.get(uid, {});
			let tree = {};
			for (const [internal, value] of Object.entries(data)) {
				const paths = KeyMapping.internalToKey(internal);
				const lastSegment = paths.pop();
				const object = paths.reduce((r, segment) => {
					r[segment] ??= {};
					return r[segment];
				}, tree);
				object[lastSegment] = value.values[0];
			}
			return tree;
		});
		this.cache = cache.getAccessor("data");
	}

	async *getNodes() {
		const it = CollectionPaging.makeIterator(async (maxOrPaging) => {
			return await this.storage.list(maxOrPaging);
		}, 50);
		for await (const [name, _] of it) {
			yield name;
		}
	}

	async get(uid) {
		return new Node(this.storage, this.cache, uid, this.handlers);
	}
}
