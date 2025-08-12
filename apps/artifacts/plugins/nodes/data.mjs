import Cache2 from "#bzd/nodejs/core/cache2.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Optional from "#bzd/nodejs/utils/optional.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");

/// Structure to contain the data.
///
/// Use to facilitate access to the data by keys
/// and reduce storage access.

const SPECIAL_KEY_FOR_VALUE = "\x01";

export default class Data {
	constructor(options) {
		this.options = Object.assign(
			{
				cache: new Cache2(),
				external: (uid, internal, count, after, before) => {
					return [];
				},
			},
			options,
		);
		this.storage = {};

		this.options.cache.register("tree", async (uid) => {
			// Convert data into a tree with leaf being the internal key.
			// {
			//    a: {SPECIAL_KEY_FOR_VALUE: "a"},
			//    b: { c: { SPECIAL_KEY_FOR_VALUE: "b.c", "d": { SPECIAL_KEY_FOR_VALUE: "b.c.d" } }},
			//    a.average: {SPECIAL_KEY_FOR_VALUE: "a.average"},
			// }
			// Using SPECIAL_KEY_FOR_VALUE for the key, enables nested keys form leaf nodes.
			//
			const data = this.storage[uid] || {};
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
		this.tree = this.options.cache.getAccessor("tree");
	}

	/// Get the timestamp.
	static getTimestamp() {
		return Date.now();
	}

	/// List all available keys
	async list() {
		return Object.keys(this.storage);
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

	/// Get the array of keys for this specific entry, which are equal and children of `key`.
	async getKeys_(uid, key, children) {
		const data = await this.getTree(uid, key);
		if (data === null) {
			return null;
		}

		const keys = [];
		const treeToKeys = (tree, children) => {
			if (typeof tree === "string") {
				keys.push(tree);
			} else {
				if (children > 0) {
					Object.entries(tree).forEach(([_, v]) => {
						treeToKeys(v, children - 1);
					});
				}
			}
		};
		treeToKeys(data, children + 1); // +1 for '_'.

		return keys;
	}

	/// Fetch data from external source.
	///
	/// The value might correspond to a time series, where the newest values come first.
	///
	/// \return An array of tuple, containing the timestamps and their corresponding value.
	///         An empty array is returned if this data does not exists.
	async getExternal_({ uid, internal, count, after = null, before = null }) {
		return await this.options.external(uid, internal, count, after, before);
	}

	/// Get a single key/value paur with its metadata.
	///
	/// The value might correspond to a time series, where the newest values come first.
	///
	/// \return An array of tuple, containing the timestamps and their corresponding value.
	///         Or null, if there is reference to this data (wrong uid/internal).
	async getWithMetadata_({ uid, internal, count, after = null, before = null }) {
		const data = this.storage[uid] || {};

		// If there is data locally.
		if (internal in data && data[internal].length) {
			const value = data[internal];

			if (after !== null && before !== null) {
				Exception.assertPrecondition(after < before, "after ({}) must be lower than before ({}).", after, before);
				Exception.unreachable("unsupported yet.");
			}

			// Gather newer data than 'after' timestamp.
			if (after !== null) {
				const end = value.findLastIndex((d) => d[0] > after);

				// Every samples are newer than what is requested.
				if (end == -1) {
					return [];
				}

				let result = value.slice(Math.max(end - count + 1, 0), end + 1);

				// If it matches the last element, it might be that older elements are also matching.
				if (end == value.length - 1) {
					const external = await this.getExternal_({ uid, internal, after, count });
					result = result.concat(external);
				}

				return result.slice(-count);
			}

			// Gather older data than 'before' timestamp.
			if (before !== null) {
				const start = value.findIndex((d) => d[0] < before);

				// No local data are older.
				if (start == -1) {
					return await this.getExternal_({ uid, internal, before, count });
				}

				let result = value.slice(start, start + count);

				// Get extra data from the external source if needed.
				if (result.length < count) {
					const external = await this.getExternal_({ uid, internal, before, count: count - result.length });
					result = result.concat(external);
				}

				return result;
			}

			return value.slice(0, count);
		}

		return null;
	}

	/// Get all keys/value pair children of key.
	async get({ uid, key, metadata = false, children = 0, count = null, after = null, before = null, include = null }) {
		const valuesToResult = (values) => {
			values = values.map(([t, v]) => {
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
					include.map(async (subKey) => await this.getKeys_(uid, [...key, ...subKey], children)),
				);
				keys = [...new Set(arrayOfSets.filter((a) => a !== null).reduce((a, c) => a.concat([...c]), []))];
			} else if (include) {
				keys = [...new Set(include.map((relative) => KeyMapping.keyToInternal([...key, ...relative])))];
			} else {
				keys = await this.getKeys_(uid, key, children);
			}

			if (keys !== null) {
				// Get all values in parallel.
				const valuesWithMetadata = await Promise.all(
					keys.map((internal) => this.getWithMetadata_({ uid, internal, count: count || 1, after, before })),
				);

				let allValues = [];
				for (const index in keys) {
					const [values, internal] = [valuesWithMetadata[index], keys[index]];
					// Filter out entries that are empty.
					if (values && values.length) {
						const processedValues = valuesToResult(values);
						allValues.push([KeyMapping.internalToKey(internal).slice(key.length), processedValues]);
					}
				}
				return new Optional(allValues);
			}
		}
		// Get the value directly.
		else {
			const internal = KeyMapping.keyToInternal(key);
			const values = await this.getWithMetadata_({ uid, internal, count: count || 1, after, before });
			if (values !== null) {
				return new Optional(valuesToResult(values));
			}
		}

		return new Optional();
	}

	/// Insert the given fragments.
	///
	/// \param uid The uid to update.
	/// \param fragments An iterable of tuples, which first element is the absolute key and second the value to be inserted.
	/// \param timestamp The timestamp to be used.
	///
	/// \return The timestamp actually written.
	async insert(uid, fragments, timestamp = null) {
		timestamp = timestamp === null ? Data.getTimestamp() : timestamp;
		this.storage[uid] ??= {};
		let data = this.storage[uid];

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
			// Insert "internal" -> [timestamp, value]
			data[internal].splice(index, 0, [timestamp, value]);
			while (data[internal].length > config.history) {
				data[internal].pop();
			}
		}

		return timestamp;
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
