import Cache2 from "#bzd/nodejs/core/cache2.js";
import KeyMapping from "#bzd/nodejs/db/data/key_mapping.js";
import Optional from "#bzd/nodejs/utils/optional.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { timestampMs } from "#bzd/nodejs/utils/timestamp.js";

const Exception = ExceptionFactory("db", "data");

/// Structure to contain the data.
///
/// Use to facilitate access to the data by keys
/// and reduce storage access.

const SPECIAL_KEY_FOR_VALUE = "\x01";

export default class Data {
	constructor(options) {
		this.options = Object.assign(
			{
				/// The cache instance to be used.
				cache: new Cache2(),
				/// The external source to fetch if data is missing locally.
				external: (uid, key, count, after, before) => {
					return null;
				},
				/// Callback when creatinga new uid or a new key.
				newBranch: (uid, key) => {},
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
			let tree = {};
			if (uid in this.storage) {
				for (const [internal, _] of Object.entries(this.storage[uid].data)) {
					const paths = KeyMapping.internalToKey(internal);
					const object = paths.reduce((r, segment) => {
						r[segment] ??= {};
						return r[segment];
					}, tree);
					object[SPECIAL_KEY_FOR_VALUE] = { internal: internal, key: paths };
				}
			}
			return tree;
		});
		this.tree = this.options.cache.getAccessor("tree");
	}

	/// List all available uids and their metadata.
	getEntries() {
		return Object.fromEntries(Object.entries(this.storage).map(([uid, storage]) => [uid, storage.metadata]));
	}

	/// Helper to access an internal value.
	getDataInternal_(uid, key, internal) {
		if (!(uid in this.storage)) {
			this.storage[uid] = {
				metadata: {},
				data: {},
			};
			this.options.newBranch(uid, []);
		}
		if (!(internal in this.storage[uid].data)) {
			this.storage[uid].data[internal] = {
				expiresType: "auto",
				expires: 60, // seconds
				unit: "",
				values: [],
			};
			this.options.newBranch(uid, key);
			this.tree.setDirty(uid);
		}
		return this.storage[uid].data[internal];
	}

	/// Get the tree at a given key.
	///
	/// \return the sub-tree which root is the key if this key is part of the tree,
	///         or null otherwise.
	async getTree_(uid, key) {
		const data = await this.tree.get(uid);
		const reducedData = key.reduce((r, segment) => {
			if (r === null || !(segment in r) || segment == SPECIAL_KEY_FOR_VALUE) {
				return null;
			}
			return r[segment];
		}, data);
		return reducedData;
	}

	/// Get the list of keys for this specific entry, which are equal and children of `key`.
	///
	/// \note The keys are sorted, useful to get consistent results.
	///
	/// \param uid The uid to get the keys for.
	/// \param key The key to get the children of.
	/// \param children The number of children to reach.
	/// \param includeInner If true, the inner nodes are included, otherwise only the leaf nodes.
	///
	/// \return A dictionary of keys, or null if the key is not part of the tree.
	async getKeys_(uid, key, children, includeInner = true) {
		const data = await this.getTree_(uid, key);
		if (data === null) {
			return null;
		}

		const keys = [];
		const treeToKeys = (tree, children, key) => {
			if (children > 0) {
				const sortedKeys = Object.keys(tree).sort();
				sortedKeys.forEach((k) => {
					const v = tree[k];
					if (k == SPECIAL_KEY_FOR_VALUE) {
						if (!includeInner) {
							keys.push({
								internal: v.internal,
								key: v.key,
								leaf: true,
							});
						}
					} else {
						const subKey = key.concat(k);
						treeToKeys(v, children - 1, subKey);
						if (includeInner) {
							const isLeaf = SPECIAL_KEY_FOR_VALUE in v;
							keys.push({
								key: subKey,
								leaf: isLeaf,
								internal: isLeaf ? v[SPECIAL_KEY_FOR_VALUE].internal : null,
							});
						}
					}
				});
			}
		};
		treeToKeys(data, children + 1, key); // +1 for '_'.

		return keys;
	}

	/// Fetch data from external source.
	///
	/// The value might correspond to a time series, where the newest values come first.
	///
	/// \return An array of tuple, containing the timestamps and their corresponding value.
	///         Or null, if there is reference to this data (wrong uid/internal).
	async getExternal_({ uid, key, count, after = null, before = null }) {
		return await this.options.external(uid, key, count, after, before);
	}

	/// Get a single key/value paur with its metadata.
	///
	/// The value might correspond to a time series, where the newest values come first.
	///
	/// \return An array of tuple, containing the timestamps and their corresponding value.
	///         Or null, if there is reference to this data (wrong uid/internal).
	async getWithMetadata_({ uid, key, value, count, after = null, before = null, sampling = null }) {
		// If there is data locally.
		if (value && value.length) {
			if (after !== null && before !== null) {
				Exception.assertPrecondition(after < before, "after ({}) must be lower than before ({}).", after, before);

				const start = value.findIndex((d) => d[0] < before);
				const end = value.findLastIndex((d) => d[0] > after);

				// Every samples are newer than what is requested.
				if (end == -1) {
					return [];
				}

				// 'before' is older than the local data, means every samples are older.
				if (start == -1) {
					const external = await this.getExternal_({ uid, key, count, after, before });
					return external === null ? [] : external;
				}

				// Get the local data.
				let result = value.slice(start, end + 1);

				// If more data is requested, it is ensured that there is at least one sample here.
				if (end == value.length - 1) {
					// Calculate how many data are needed from the local/external.
					const oldestLocal = value[end][0];
					const durationLocal = before - oldestLocal;
					const countLocal = Math.round((count * durationLocal) / (before - after)) || 1;
					const countExternal = count - countLocal;

					// Downsample the local data (we don't want to upsample, create artifial data).
					// This simple algorithm do not alter the samples and dowsample to the exact number
					// of samples as the factor is guaranteed to be > 1.
					if (countLocal < result.length) {
						const downsamplingFactor = result.length / countLocal;
						let downsampleResult = [];
						const samplingFct = sampling
							? {
									newest: Math.floor,
									oldest: Math.ceil,
								}[sampling]
							: Math.round;
						Exception.assertPrecondition(samplingFct, "Unsupported sampling mode '{}'", sampling);
						for (let index = 0; Math.round(index) < result.length; index += downsamplingFactor) {
							downsampleResult.push(result[samplingFct(index)]);
						}
						result = downsampleResult;
					}

					// Get the rest of the data from the external source.
					const external = await this.getExternal_({ uid, key, count: countExternal, after, before: oldestLocal });
					result = external === null ? result : result.concat(external);
				}

				return result;
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
					const external = await this.getExternal_({ uid, key, after, count });
					if (external !== null && external.length) {
						const newestExternal = external[0][0];
						const endLocal = result.findLastIndex((d) => newestExternal < d[0]);
						result = result.slice(0, endLocal + 1).concat(external);
					}
				}

				return result.slice(-count);
			}

			// Gather older data than 'before' timestamp.
			if (before !== null) {
				const start = value.findIndex((d) => d[0] < before);

				// No local data are older.
				if (start == -1) {
					const external = await this.getExternal_({ uid, key, before, count });
					return external === null ? [] : external;
				}

				let result = value.slice(start, start + count);

				// Get extra data from the external source if needed.
				if (result.length < count) {
					const external = await this.getExternal_({
						uid,
						key,
						before: result.length ? result.at(-1)[0] : before,
						count: count - result.length,
					});
					result = external === null ? result : result.concat(external);
				}

				return result;
			}

			let result = value.slice(0, count);
			if (result.length < count) {
				const external = await this.getExternal_({ uid, key, count: count - result.length });
				result = external === null ? result : result.concat(external);
			}
			return result;
		}

		return await this.getExternal_({ uid, key, count, after, before });
	}

	/// Get all keys/value pair children of key.
	async get({
		uid,
		key,
		metadata = false,
		children = 0,
		count = null,
		after = null,
		before = null,
		include = null,
		sampling = null,
	}) {
		const data = uid in this.storage ? this.storage[uid].data : {};

		const valuesToResult = (key, internal, values) => {
			const dataInternal = this.getDataInternal_(uid, key, internal);
			if (metadata) {
				let result = values.map(([t, v]) => [t, v]);
				if (result.length > 0) {
					// Metadata is only attached to the most recent entry.
					result[0][2] = dataInternal.expires;
					result[0][3] = dataInternal.unit;
				}
				return result;
			}
			const expiredTimestampMs = timestampMs() - dataInternal.expires * 1000;
			return values
				.filter(([t, _]) => {
					return t > expiredTimestampMs;
				})
				.map(([_, v]) => {
					return v;
				});
		};

		// Get the list of keys.
		if (children || include) {
			let keys = null;
			if (children && include) {
				const arrayOfArrays = await Promise.all(
					include.map(
						async (subKey) => await this.getKeys_(uid, [...key, ...subKey], children, /*includeInner*/ false),
					),
				);
				keys = arrayOfArrays.filter((item) => item !== null).flat();
			} else if (include) {
				keys = include.map((relative) => {
					return {
						internal: KeyMapping.keyToInternal([...key, ...relative]),
						key: [...key, ...relative],
					};
				});
			} else {
				keys = await this.getKeys_(uid, key, children, /*includeInner*/ false);
			}

			if (keys !== null) {
				// Get all values in parallel.
				const valuesWithMetadata = await Promise.all(
					keys.map((child) =>
						this.getWithMetadata_({
							uid,
							key: child.key,
							value: data[child.internal]?.values,
							count: count || 1,
							after,
							before,
							sampling,
						}),
					),
				);

				const allValues = keys
					.map((child, index) => {
						const values = valuesWithMetadata[index];
						// Filter out entries that are empty.
						if (values && values.length) {
							const result = valuesToResult(child.key, child.internal, values);
							if (result.length) {
								return [child.key.slice(key.length), count === null ? result[0] : result];
							}
						}
						return null;
					})
					.filter((x) => x !== null);

				return new Optional(allValues);
			}
		}
		// Get the value directly.
		else {
			const internal = KeyMapping.keyToInternal(key);
			const values = await this.getWithMetadata_({
				uid,
				key,
				value: data[internal]?.values,
				count: count || 1,
				after,
				before,
				sampling,
			});
			if (values !== null) {
				const result = valuesToResult(key, internal, values);
				return new Optional(count === null ? result[0] : result);
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
	insert(uid, fragments, timestamp = null) {
		timestamp = timestamp === null ? timestampMs() : timestamp;

		// Identify the path of the fragments and their values.
		for (const [key, value, options] of fragments) {
			const config = Object.assign(
				{
					// The number of values to be kept as history.
					history: 10,
					// The duration in seconds until which an entry is considered expired.
					expires: null,
					// The unit associated with this entry, the unit is expected to be in UCUM format.
					unit: null,
				},
				options,
			);

			const data = this.getDataInternal_(uid, key, KeyMapping.keyToInternal(key));

			let index = 0;

			if (data.values.length) {
				// If the timestamp of the last entry added is newer than the current one.
				if (data.values[0][0] > timestamp) {
					index = data.values.findIndex((d) => d[0] <= timestamp);
					if (index == -1) {
						index = data.values.length;
					}
				}
				// If there is already data and the espiration is set as "auto".
				else if (data.expiresType == "auto") {
					// Estimate the rate and estimate the expiration rate.
					// The expiration is rounded to avoid long floats when send via HTTP,
					// the value is minimum 1s (as it doesn't make sense to have it 0 or a lower granularity.
					const expiresEstimate = ((timestamp - data.values[0][0]) * 3) / 1000;
					data.expires = Math.round(0.4 * data.expires + 0.6 * expiresEstimate) || 1;
				}
			}

			// Prepend the new value and the timestamp to the values array.
			// And ensure there are maximum X elements.
			// Insert "internal" -> [timestamp, value]
			data.values.splice(index, 0, [timestamp, value]);
			while (data.values.length > config.history) {
				data.values.pop();
			}

			// Set the unit if requested.
			if (config.unit) {
				data.unit = config.unit;
			}

			// Set the expiry if requested.
			if (config.expires) {
				if (typeof config.expires == "number") {
					data.expiresType = "manual";
					data.expires = config.expires;
				} else {
					data.expiresType = "auto";
				}
			}
		}

		return timestamp;
	}

	/// Get direct children of a given key.
	///
	/// \return A dictionary which keys are the name of the children and value a boolean describing if
	///         the data is nested of a leaf.
	async getChildren({ uid, key, children, includeInner }) {
		const data = await this.getKeys_(uid, key, children, includeInner);
		if (data === null) {
			return null;
		}
		return data.map((child) => {
			return {
				key: child.key.slice(key.length),
				leaf: child.leaf,
			};
		});
	}
}
