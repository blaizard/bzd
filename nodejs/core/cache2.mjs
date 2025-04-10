import ExceptionFactory from "./exception.mjs";
import LogFactory from "./log.mjs";
import ServiceProvider from "#bzd/nodejs/core/services/provider.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";

const Log = LogFactory("cache");
const Exception = ExceptionFactory("cache");

export default class Cache2 {
	constructor(options) {
		this.options = Object.assign(
			{
				// Default timeout for the validity of an entry.
				timeoutMs: 30 * 1000,
				// Default function to compute the size of the value.
				getSize: Cache2.defaultGetSize,
				// Max size allowed for the cache.
				maxSize: 1024 * 1024,
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
		//          data: undefined,
		//          status: <Statue>,
		//          timeout: <int>,
		//          size: undefined,
		//      }}
		// }
		this.data = {};
		this.size = 0;
		this.trigger = null;
	}

	/// Defines an empty state.
	static empty = Symbol();

	/// Status of a entry.
	static Status = Object.freeze({
		value: Symbol("value"),
		error: Symbol("error"),
		fetching: Symbol("fetching"),
	});

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

	/// Default function to get a very rought estimate of the size of an object.
	///
	/// This function advantages speed of accuracy.
	/// It is not exact at all but gives a sense of whether a value takes a large
	/// memory chunk or not and this is all what we need for the eviciton mechanism
	/// with the cache.
	static defaultGetSize(value) {
		const sizeFactory = {
			number: () => 8,
			undefined: () => 8,
			object: (value) => (value === null ? 8 : JSON.stringify(value).length),
			boolean: () => 1,
			bigint: (value) => String(value).length,
			string: (value) => value.length,
			symbol: () => 8,
			function: () => 32,
		};
		return sizeFactory[typeof value](value);
	}

	/// Register a new data collection.
	///
	/// \param collection The name of the collection.
	/// \param fetch A function to fetch the new data.
	/// \param options Options specific to this collection.
	register(collection, fetch, options) {
		Exception.assert(!(collection in this.data), "Collection '{}' already registered.", collection);
		Log.info("Registering collection '{}'.", collection);
		this.data[collection] = {
			fetch: fetch,
			options: Object.assign(
				{
					// Default timeout for the validity of an entry.
					timeoutMs: this.options.timeoutMs,
					// Get the size of the value.
					getSize: this.options.getSize,
				},
				options,
			),
			// Here a map is used instead of the dictionary for the following reasons:
			// 1. Performance: It performs better in scenarios involving frequent additions and removals of key-value pairs.
			// 2. Key order: The keys in Map are ordered in a straightforward way: A Map object iterates entries, keys, and values in the order of entry insertion.
			values: new Map(),
			// Size for this collection.
			size: 0,
		};
	}

	/// Manually set a value for the collection.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	/// \param value The value to be set.
	/// \param type The data type to be set.
	set(collection, key, value, type = Cache2.Status.value) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];

		// Touch the value, this is important to set the value on top.
		let sizeDiff = -(data.values.get(key) ?? { size: 0 }).size;
		data.values.delete(key);

		let size = 0;
		switch (type) {
			case Cache2.Status.value:
				size = data.options.getSize(value);
				break;
			case Cache2.Status.error:
				size = 8;
				break;
			default:
				Exception.unreachable("Cannot set the value for {}::{} to type '{}'.", collection, key, type);
		}
		data.values.set(key, {
			data: value,
			status: type,
			size: size,
			timeout: Cache2.getTimestampMs() + data.options.timeoutMs,
		});
		sizeDiff += size;
		data.size += sizeDiff;
		this.size += sizeDiff;

		if (this.trigger && this.size > this.options.maxSize) {
			this.trigger.trigger();
		}
	}

	/// Access a value or fetch a new value.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	async get(collection, key) {
		const data = this.data[collection];
		while (true) {
			// Check if the value is already available.
			const maybeValue = this.getInstant(collection, key, Cache2.empty);
			if (maybeValue !== Cache2.empty) {
				return maybeValue;
			}

			// Handle the case of concurrency, if the value is being fetched and another caller issues a get(...).
			// Check if the value is being fetched.
			const isFetching = (data.values.get(key) ?? { status: null }).status === Cache2.Status.fetching;
			if (!isFetching) {
				break;
			}

			const wait = async () => {
				return new Promise((resolve) => {
					data.values.get(key).data.push(resolve);
				});
			};
			await wait();
		}

		// We are the first async thread to fetch this entry.
		// Mark this entry as fetching.
		let resolveList = [];
		data.values.set(key, {
			// Setting the timeout to -1, will tell that this entry is invalid and needs refetch.
			timeout: -1,
			data: resolveList, // contains the resolve callback of all promises waiting.
			status: Cache2.Status.fetching,
			size: (data.values.get(key) ?? { size: 0 }).size,
		});

		try {
			const value = await data.fetch(key);
			this.set(collection, key, value, Cache2.Status.value);
		} catch (e) {
			this.set(collection, key, e, Cache2.Status.error);
			throw e;
		} finally {
			resolveList.forEach((resolve) => resolve());
		}

		return data.values.get(key).data;
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
		if (entry.status === Cache2.Status.error) {
			throw entry.data;
		}

		// Touch the value.
		data.values.delete(key);
		data.values.set(key, entry);

		return entry.data;
	}

	/// Mark the current data as invalidated (out of date) so it will be reloaded at the next access.
	///
	/// \param collection The collection to get data from.
	/// \param key The key of the data.
	setDirty(collection, key) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		if (data.values.has(key)) {
			data.values.get(key).timeout = -1;
		}
	}

	/// Get the size.
	///
	/// \param collection The collection to get data from.
	///
	/// \return The size of the data.
	getSize(collection = null) {
		if (collection === null) {
			return this.size;
		}
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		return data.size;
	}

	/// Register the garbage collector service.
	serviceGarbageCollector(...namespaces) {
		Exception.assert(this.trigger === null, "Garbage collector is already registered.");
		const provider = new ServiceProvider(...namespaces);
		this.trigger = provider.addEventTriggeredProcess("garbage.collector", async () => {
			return {};
		});
		return provider;
	}

	/// Register the statistics for this cache.
	statistics(...namespaces) {
		return new StatisticsProvider(...namespaces);
	}
}
