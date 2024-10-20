import Cache from "#bzd/nodejs/core/cache.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Optional from "#bzd/nodejs/utils/optional.mjs";

/// Structure to contain the data.
///
/// Use to facilitate access to the data by keys
/// and reduce storage access.

const SPECIAL_KEY_FOR_VALUE = "\x01";

export default class Data {
	constructor(storage) {
		this.storage = storage;
		const cache = new Cache({
			garbageCollector: false,
		});

		cache.register("tree", async (uid) => {
			// Convert data into a tree with leaf being the internal key.
			// {
			//    a: {SPECIAL_KEY_FOR_VALUE: "a"},
			//    b: { c: { SPECIAL_KEY_FOR_VALUE: "b.c", "d": { SPECIAL_KEY_FOR_VALUE: "b.c.d" } }},
			//    a.average: {SPECIAL_KEY_FOR_VALUE: "a.average"},
			// }
			// Using SPECIAL_KEY_FOR_VALUE for the key, enables nested keys form leaf nodes.
			//
			const data = await this.storage.get(uid, {});
			let tree = {};
			for (const [internal, _] of Object.entries(data)) {
				const paths = KeyMapping.internalToKey(internal);
				const object = paths.reduce((r, segment) => {
					r[segment] ??= {};
					return r[segment];
				}, tree);
				object[SPECIAL_KEY_FOR_VALUE] = internal;
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
			if (r === null || !(segment in r) || segment == SPECIAL_KEY_FOR_VALUE) {
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
	async get(uid, key, metadata = false, children = false, count = null, after = null, before = null, include = null) {
		const data = await this.storage.get(uid, {});

		const getStartEnd = (value) => {
			if (after !== null) {
				const end = value.findLastIndex((d) => d[0] > after);
				if (end == -1) {
					return [0, 0];
				}
				return [Math.max(end - (count || 1) + 1, 0), end + 1];
			}

			if (before !== null) {
				const start = value.findIndex((d) => d[0] < before);
				if (start == -1) {
					return [0, 0];
				}
				return [start, start + (count || 1)];
			}

			return [0, count || 1];
		};

		const processValue = (value) => {
			const [start, end] = getStartEnd(value);
			const values = value.slice(start, end).map(([t, v]) => {
				if (metadata) {
					return [t, v];
				}
				return v;
			});
			return count === null ? values[0] : values;
		};

		// Get the list of keys.
		if (children || include) {
			let keys = null;
			if (children && include) {
				const arrayOfSets = await Promise.all(
					include.map(async (subKey) => await this.getKeys(uid, [...key, ...subKey])),
				);
				keys = new Set(arrayOfSets.filter((a) => a !== null).reduce((a, c) => a.concat([...c]), []));
			} else if (include) {
				keys = new Set(include.map((relative) => KeyMapping.keyToInternal([...key, ...relative])));
			} else {
				keys = await this.getKeys(uid, key);
			}

			if (keys !== null) {
				const values = Object.entries(data)
					// Only includes the children keys.
					.filter(([k, _]) => keys.has(k))
					// Get the values.
					.map(([k, v]) => [KeyMapping.internalToKey(k).slice(key.length), processValue(v)])
					// Filter out entries that are empty.
					.filter(([_, v]) => count === null || v.length > 0);
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
	/// \param timestamp The timestamp to be used.
	async insert(uid, fragments, timestamp = null) {
		timestamp = timestamp === null ? Data.getTimestamp() : timestamp;

		await this.storage.update(
			uid,
			async (data) => {
				// Identify the path of the fragments and their values.
				for (const [key, value, options] of fragments) {
					const config = Object.assign(
						{
							// The number of values to be kept as history.
							history: 10,
						},
						options,
					);

					const internal = KeyMapping.keyToInternal(key);
					let index = 0;
					if (!(internal in data)) {
						data[internal] = [];
						this.tree.setDirty(uid);
					}
					// If the timestamp of the last entry added is newer than the current one.
					else if (data[internal][0][0] > timestamp) {
						index = data[internal].findIndex((d) => d[0] <= timestamp);
						if (index == -1) {
							index = data[internal].length;
						}
					}
					// Prepend the new value and the timestamp to the values array.
					// And ensure there are maximum X elements.
					data[internal].splice(index, 0, [timestamp, value]);
					while (data[internal].length > config.history) {
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
			.filter((v) => v != SPECIAL_KEY_FOR_VALUE)
			.map((v) => {
				const keys = Object.keys(data[v]);
				return [v, !(keys.length == 1 && keys[0] == SPECIAL_KEY_FOR_VALUE)];
			});

		return new Optional(Object.fromEntries(children));
	}
}
