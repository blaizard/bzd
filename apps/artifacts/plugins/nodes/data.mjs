import Cache from "#bzd/nodejs/core/cache.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Optional from "#bzd/nodejs/utils/optional.mjs";

/// Structure to contain the data.
///
/// Use to facilitate access to the data by keys
/// and reduce storage access.

export default class Data {
	constructor(storage) {
		this.storage = storage;
		const cache = new Cache({
			garbageCollector: false,
		});

		cache.register("tree", async (uid) => {
			// Convert data into a tree with leaf being the internal key.
			// {
			//    a: {"_": "a"},
			//    b: { c: { "_": "b.c", "d": { "_": "b.c.d" } }},
			//    a.average: {"_": "a.average"},
			// }
			// Using '_' for the key, enables nested keys form leaf nodes.
			//
			const data = await this.storage.get(uid, {});
			let tree = {};
			for (const [internal, _] of Object.entries(data)) {
				const paths = KeyMapping.internalToKey(internal);
				const object = paths.reduce((r, segment) => {
					r[segment] ??= {};
					return r[segment];
				}, tree);
				object["_"] = internal;
			}
			return tree;
		});
		this.tree = cache.getAccessor("tree");
	}

	/// Get the timestamp.
	static getTimestamp() {
		return Date.now();
	}

	/// Get the tree at a given key.
	///
	/// \return the sub-tree which root is the key if this key is part of the tree,
	///         or null otherwise.
	async getTree(uid, key) {
		const data = await this.tree.get(uid);
		const reducedData = key.reduce((r, segment) => {
			if (r === null || !(segment in r)) {
				return null;
			}
			return r[segment];
		}, data);
		return reducedData;
	}

	/// Get the set of keys for this specific entry, which are equal and children of `key`.
	async getKeys(uid, key) {
		const data = await this.getTree(uid, key);
		if (data === null) {
			return null;
		}

		const keys = new Set();
		const treeToKeys = (tree) => {
			if (typeof tree === "string") {
				keys.add(tree);
			} else {
				Object.entries(tree).forEach(([_, v]) => {
					treeToKeys(v);
				});
			}
		};
		treeToKeys(data);

		return keys;
	}

	///  Get all keys/value pair children of key.
	async get(uid, key, metadata = false, children = false, count = null) {
		const data = await this.storage.get(uid, {});

		const processValue = (value) => {
			const values = value.slice(0, count || 1).map(([t, v]) => {
				if (metadata) {
					return [t, v];
				}
				return v;
			});
			return count === null ? values[0] : values;
		};

		// Get the list of keys.
		if (children) {
			const keys = await this.getKeys(uid, key);
			if (keys !== null) {
				const values = Object.entries(data)
					.filter(([k, _]) => keys.has(k))
					.map(([k, v]) => [KeyMapping.internalToKey(k).slice(key.length), processValue(v)]);
				return new Optional(values);
			}
		}
		// Get the value directly.
		else {
			const internal = KeyMapping.keyToInternal(key);
			if (internal in data) {
				return new Optional(processValue(data[internal]));
			}
		}

		return new Optional();
	}

	/// Insert the given fragments.
	///
	/// \param uid The uid to update.
	/// \param fragments An iterable of tuples, which first element is the absolute key and second the value to be inserted.
	async insert(uid, fragments) {
		const timestamp = Data.getTimestamp();

		await this.storage.update(
			uid,
			async (data) => {
				// Identify the path of the fragments and their values.
				for (const [key, value] of fragments) {
					const internal = KeyMapping.keyToInternal(key);
					if (!(internal in data)) {
						data[internal] = [];
						this.tree.setDirty(this.uid);
					}
					// Prepend the new value and the timestamp to the values array.
					// And ensure there are maximum X elements.
					data[internal].unshift([timestamp, value]);
					while (data[internal].length > 10) {
						//this.handlers.process("history", internal)) {
						data[internal].pop();
					}
				}

				return data;
			},
			{},
		);

		return true;
	}

	/// Get direct children of a given key.
	///
	/// \return A dictionary which keys are the name of the children and value a boolean describing if
	///         the data is nested of a leaf.
	async getChildren(uid, key) {
		const data = await this.getTree(uid, key);
		if (data === null) {
			return new Optional();
		}

		const children = Object.keys(data)
			.filter((v) => v != "_")
			.map((v) => {
				const keys = Object.keys(data[v]);
				return [v, !(keys.length == 1 && keys[0] == "_")];
			});

		return new Optional(Object.fromEntries(children));
	}
}
