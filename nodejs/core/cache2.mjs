import ExceptionFactory from "./exception.mjs";
import LogFactory from "./log.mjs";

const Log = LogFactory("cache");
const Exception = ExceptionFactory("cache");

export default class Cache2 {
	constructor(options) {
		this.options = Object.assign(
			{
				// Default timeout for the validity of an entry.
				timeoutMs: 30 * 1000,
			},
			options,
		);

		// Contains all the cached entries.
		//
		// Each entry has the following format:
		// - [collection]: {
		//      fetch: <function>,
		//      options: {
		//          ....
		//      },
		//      values: { [key]: {
		//          value: undefined,
		//          error: undefined,
		//          size: undefined,
		//          timeout: undefined
		//      }}
		// }
		this.data = {};
	}

	/// Defines an empty state.
	static empty = Symbol();

	/// Current timestamp in milliseconds.
	static getTimestampMs() {
		return Date.now();
	}

	/// Helper to convert an array of string into a key.
	static arrayOfStringToKey(array) {
		return ["", ...array].join("\0");
	}

	/// Helper to convert a key into an array of string.
	static keyToArrayOfString(payload) {
		return payload.split("\0").slice(1);
	}

	/// Register a new data collection.
	///
	/// \param collection The name of the collection.
	/// \param fetch A function to fetch the new data.
	/// \param options Options specific to this collection.
	register(collection, fetch, options) {
		Exception.assert(!(collection in this.data), "Collection '{}' already registered.", collection);
		this.data[collection] = {
			fetch: fetch,
			options: Object.assign(
				{
					// Default timeout for the validity of an entry.
					timeoutMs: this.options.timeoutMs,
					// Get the size of the value.
					getSize: undefined,
				},
				options,
			),
			// Here a map is used instead of the dictionary for the following reasons:
			// 1. Performance: It performs better in scenarios involving frequent additions and removals of key-value pairs.
			// 2. Key order: The keys in Map are ordered in a straightforward way: A Map object iterates entries, keys, and values in the order of entry insertion.
			values: new Map(),
		};
	}

	/// Manually set a value for the collection.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	/// \param value The value to be set.
	set(collection, key, value) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		// Touch the value, this is important to set the value on top.
		data.values.delete(key);
		data.values.set(key, {
			value: value,
			size: data.options.getSize ? data.options.getSize(value) : undefined,
			timeout: Cache2.getTimestampMs() + data.options.timeoutMs,
		});
	}

	/// Access a value or fetch a new value.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	async get(collection, key) {
		// Check if the value is already available.
		const maybeValue = this.getInstant(collection, key, Cache2.empty);
		if (maybeValue !== Cache2.empty) {
			return maybeValue;
		}

		// TODO: handle the case of concurrency, if the value is being fetched
		// and another caller issues a get(...).

		const data = this.data[collection];
		try {
			const value = await data.fetch(key);
			this.set(collection, key, value);
		} catch (e) {
			data.values.set(key, {
				error: e,
				timeout: Cache2.getTimestampMs() + data.options.timeoutMs,
			});
			throw e;
		}
		return data.values.get(key).value;
	}

	/// Get the value instantly if available.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	/// \param defaultValue The default value if the data is not available.
	getInstant(collection, key, defaultValue) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		if (!data.values.has(key)) {
			return defaultValue;
		}
		const entry = data.values.get(key);
		if (entry.timeout < Cache2.getTimestampMs()) {
			return defaultValue;
		}
		if (entry.error) {
			throw data.values[key].error;
		}

		// Touch the value.
		data.values.delete(key);
		data.values.set(key, entry);

		return entry.value;
	}

	/// Mark the current data as invalidated (out of date) so it will be reloaded at the next access.
	///
	/// \param collection The collection to get data from.
	/// \param key The key of the data.
	setDirty(collection, key) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		if (data.values.has(key)) {
			data.values.get(key).timeout = Cache2.getTimestampMs() - 1;
		}
	}
}
