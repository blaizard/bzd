"use strict";

const Event = require("./event.js");
const Log = require("./log.js")("cache");
const Exception = require("./exception.js")("cache");

/**
 * Cache a collection of data.
 * A collection is a dictionary of data that obey to the same rules (same trigger functions, timeout, etc).
 * Data inside collections are accessed by their id.
 */
class Cache
{
	constructor(config) {
		this.config = Object.assign({
			maxSize: 1000000, // 1MB
			maxEntries: 100,
			garbageCollectorPeriodMs: 10000 // Every 10s
		}, config);

		this.data = {
			_size: 0,
			_nbEntries: 0
		};

		this.resourceMap = {};

		this.event = new Event({
			error: {proactive: true}
		});

		// Run the garbage collector asynchrounsly
		garbageCollector.call(this);
	}

	/**
	 * \brief Register a new data collection
	 */
	register(collection, trigger, options) {

		Exception.assert(typeof collection === "string", "register(...) must take a string as first argument.");
		Exception.assert(typeof trigger === "function", "register(...) must take a function as second argument.");
		Exception.assert(collection.match(/^[a-z0-9\.-]+$/i), "Invalid collection name \"" + collection + "\", it must contain only the following characters [a-z0-9.-].");
		Exception.assert(!this.data.hasOwnProperty(collection), "The collection \"" + collection + "\" has already been registered.");

		this.data[collection] = {
			_trigger: trigger,
			_size: 0,
			_defaultSize: ((options && options.hasOwnProperty("size")) ? options.size : 0),
			_nbEntries: 0
		};

		// Build the value object;
		if (options && options.hasOwnProperty("timeout")) {
			this.data[collection]._timeoutMs = options.timeout;
		}

		// Set initial value if any defined and set its timeout to
		// the past to ensure that a new value will be fetched.
	//	if (options && typeof options.default !== "undefined") {
	//		data._data = options.default;
	//		data._timeout = Cache.getTimestampMs() - 1;
	//	}

		Log.info("Register collection \"" + collection + "\" to cache: {" + this.toString(collection) + "}");
	}

	/**
	 * Current timestamp
	 */
	static getTimestampMs() {
		return Date.now();
	}

	/**
	 * \brief Get the data requested. Wait until it is available.
	 *
	 * \param collection The collection to get data from
	 * \param ids The identifier of the data
	 *
	 * \return The data or a promise that will return the data.
	 */
	async get(collection, ...ids) {
		return get.call(this, /*instant*/false, collection, ...ids);
	}

	/**
	 * \brief Get the data requested.
	 * Return the old dat if available while trigger the update of the new.
	 *
	 * \param collection The collection to get data from
	 * \param ids The identifier of the data
	 *
	 * \return The data or a promise that will return the data.
	 */
	async getInstant(collection, ...ids) {
		return get.call(this, /*instant*/true, collection, ...ids);
	}

	/**
	 * \brief Get the size of the data at the specific location.
	 * If not data, return 0.
	 *
	 * \param collection The collection to get data from
	 * \param ids The identifier of the data
	 *
	 * \return The size of the data.
	 */
	getSize(collection, ...ids) {
		const obj = getObject.call(this, collection, ...ids);
		return (obj) ? (obj._size || 0) : 0;
	}

	/**
	 * \brief Delete a specific resource from the cache.
	 *
	 * \param collection The collection to get data from
	 * \param ids The identifier of the data
	 */
	async delete(collection, ...ids) {
		const id = idsToId.call(this, collection, ...ids);
		Log.debug("DELETE " + collection + "::" + id);

		if (this.data[collection].hasOwnProperty(id)) {

			// Update the size
			const size = this.data[collection][id]._size || 0;
			this.data[collection]._size -= size;
			this.data._size -= size;

			// Update the nb elements
			if (this.data[collection][id].hasOwnProperty("_data")) {
				this.data[collection]._nbEntries--;
				this.data._nbEntries--;

				// Close the previous data if a close function is available
				if (typeof this.data[collection][id]._data.close === "function") {
					await this.data[collection][id]._data.close();
				}
			}

			// Delete the entry
			delete this.data[collection][id];
		}
		delete this.resourceMap[collection + "/" + id];
	}

	/**
	 * Mark the current data as invalidated (out of date) so it will be reloaded at the next access.
	 *
	 * \param collection The collection to get data from
	 * \param ids The identifier of the data
	 */
	setDirty(collection, ...ids) {
		const id = idsToId.call(this, collection, ...ids);
		Log.debug("DIRTY " + collection + "::" + id);

		if (this.data[collection].hasOwnProperty(id)) {
			// Invalidate the data by setting its timeout in the past
			let dataId = this.data[collection][id];
			dataId._timeout = Cache.getTimestampMs() - 1;
		}
	}

	/**
	 * Show a visual representation of the current state of the cache
	 */
	toString(collection) {
		let str = "";
		if (typeof collection === "undefined") {
			str += "Cache (size=" + this.data._size + ", nbEntries=" + this.data._nbEntries + "):\n";
			for (const collection in this.data) {
				if (["_size", "_nbEntries"].indexOf(collection) == -1) {
					str += "- " + collection + ": " + this.toString(collection) + "\n";
				}
			}
		}
		else {
			const dataCollection = this.data[collection];
			if (dataCollection.hasOwnProperty("_timeoutMs")) {
				str += "timeout=" + dataCollection._timeoutMs + "ms, ";
			}
			str += "size=" + dataCollection._size + ", entries=[";
			let entryList = [];
			for (const key in dataCollection) {
				if (["_timeoutMs", "_trigger", "_size", "_nbEntries", "_defaultSize"].indexOf(key) == -1) {
					let keyInfoList = [];
					if (dataCollection[key].hasOwnProperty("_error")) {
						keyInfoList.push("error");
					}
					if (dataCollection[key].hasOwnProperty("_timeout") && dataCollection[key]._timeout < Cache.getTimestampMs()) {
						keyInfoList.push("timeout");
					}
					if (dataCollection[key].hasOwnProperty("_size")) {
						keyInfoList.push("size=" + dataCollection[key]._size);
					}
					entryList.push(key + ((keyInfoList.length) ? " (" + keyInfoList.join(", ") + ")" : ""));
				}
			}
			str += entryList.join(", ") + "]";
		}
		return str;
	}

	/**
	 * \brief Set the value of the cache.
	 *
	 * \param value The value to be set.
	 * \param ...path The identifier of the data.
	 */
	/*set(value, ...path) {
		setValue(this.data, {data: value}, "_data", ...path);
	}*/

	/**
	 * \brief Set the value of the cache.
	 *
	 * \param value The value to be set.
	 * \param timeout The timeout in Ms of the validity of the data.
	 * \param ...path The identifier of the data.
	 */
	/*setWithTimeout(value, timeout, ...path) {
		setValue(this.data, {
			data: value,
			timeout: timeout
		}, ...path);
	}*/

	/**
	 * Mark the current data as invalidated (out of date) so it will be reloaded at the next access.
	 *
	 * \param ...path The identifier of the data
	 */
	/*setDirty(...path) {
		let data = getOrCreatePath(this.data, ...path);
		delete data._data;
	}*/
};

module.exports = Cache;

// ---- Private methods -------------------------------------------------------

/**
 * \brief Return the id from an id list and perform some simple sanity checks.
 */
function idsToId(collection, ...ids)
{
	const id = (ids.length) ? ids.reduce((id, currentId) => (id + currentId), "") : "default";
	Exception.assert(id[0] != "_", "Cache::get(" + collection + ", " + id + "), ids starting with \"_\" are protected.");
	Exception.assert(typeof this.data[collection] === "object", "Cache::get(" + collection + ") does not exist or is not of type object.");

	return id;
}

/**
 * \brief Get the object at a specific id. Getting an object will not fetch data,
 * nor modify the structure.
 *
 * \param collection The collection to get data from
 * \param ids The identifier of the data
 *
 * \return The object or null if no object is present.
 */
function getObject(collection, ...ids)
{
	const id = idsToId.call(this, collection, ...ids);
	return this.data[collection][id] || null;
}

/**
 * \brief Get the data requested. Wait until it is available.
 *
 * \param instant If set, and if the data is dirty it will return it
 *        while fetching the new one. If unset, it will wait until the data is updated.
 * \param collection The collection to get data from
 * \param ids The identifier of the data
 *
 * \return The data or a promise that will return the data.
 */
async function get(instant, collection, ...ids)
{
	let status = null;
	let timestampFetch = false;
	const id = idsToId.call(this, collection, ...ids);

	// Register the access of this ressource
	touchResource.call(this, collection, id);

	do {
		const dataId = this.data[collection][id];

		// isFetching
		if (typeof dataId === "object" && dataId.hasOwnProperty("_fetching")) {
			// If instant and there are previous data, return it
			if (instant && dataId.hasOwnProperty("_data")) {
				Log.debug("GET " + collection + "::" + id + " (" + ((timestampFetch) ? ((Cache.getTimestampMs() - timestampFetch) + "ms") : "cache") + ", instant)");
				return dataId._data;
			}
			await waitForNextUpdate.call(this);
		}
		// isDirty
		else if ((typeof dataId !== "object") || (dataId.hasOwnProperty("_timeout") && dataId._timeout < Cache.getTimestampMs())) {
			timestampFetch = Cache.getTimestampMs();
			triggerUpdate.call(this, collection, id, ...ids);
		}
		// isError
		else if (dataId.hasOwnProperty("_error")) {
			Log.error("GET ERROR " + collection + "::" + id + ": " + dataId._error);
			throw new Exception(dataId._error);
		}
		// isData
		else if (dataId.hasOwnProperty("_data")) {
			Log.debug("GET " + collection + "::" + id + " (" + ((timestampFetch) ? ((Cache.getTimestampMs() - timestampFetch) + "ms") : "cache") + ")");
			return dataId._data;
		}
		else {
			throw new Exception("Invalid state " +  collection + "::" + id + ", it should never happen.");
		}
	} while (true);
}

/**
 * Register the access of a ressource
 */
function touchResource(collection, id)
{
	this.resourceMap[collection + "/" + id] = Cache.getTimestampMs();
}

function deleteResourceById(resourceId)
{
	const n = resourceId.indexOf("/");
	Exception.assert(n !== -1, "Invalid resourceId '" + resourceId + "'.");

	const collection = resourceId.substring(0, n);
	const id = resourceId.substring(n + 1);

	this.delete(collection, id);
}

/**
 * Wait for the next update
 */
async function waitForNextUpdate()
{
	return new Promise((resolve) => {
		this.event.on("updated", () => {
			resolve();
		}, /*once*/true);
	});
}

/**
 * \brief Fetch and update the data
 */
async function triggerUpdate(collection, id, ...ids)
{
	// Create the data if it does not exists
	let dataCollection = this.data[collection];
	if (!dataCollection.hasOwnProperty(id)) {
		dataCollection[id] = {};
	}
	let dataId = dataCollection[id];
	// Save the previous size
	const previousSize = dataId._size || 0;
	const previousNbEntries = (dataId.hasOwnProperty("_data")) ? 1 : 0;
	// Mark as fetching to prevent any concurrent fetching
	dataId._fetching = true;

	try {
		Exception.assert(typeof dataCollection._trigger === "function", "No trigger function associated with collection \"" + collection + "\"");

		// Close the previous data if a close function is available
		if (dataId.hasOwnProperty("_data") && typeof dataId._data.close === "function") {
			await dataId._data.close();
		}

		// Set the new data and delete the previous error if any
		dataId._data = await dataCollection._trigger(...ids, /*Previous value if any*/dataId._data);
		if (dataCollection.hasOwnProperty("_timeoutMs")) {
			dataId._timeout = Cache.getTimestampMs() + Math.max(1, dataCollection._timeoutMs);
		}
		dataId._size = dataId._data.size || dataId._data.length || dataCollection._defaultSize;
		delete dataId._error;
	}
	catch (e) {

		// Register the error and delete the previous data if any
		dataId._error = e.message;
		dataId._timeout = Cache.getTimestampMs() + 100; // Invalid the error after 100ms
		dataId._size = dataId._error.length || 0;
		delete dataId._data;
	}

	// Update the size of the collection
	dataCollection._size += (dataId._size - previousSize);
	this.data._size += (dataId._size - previousSize);

	// Update the number of entries
	const nbEntriesDelta = ((dataId.hasOwnProperty("_data")) ? 1 : 0) - previousNbEntries;
	dataCollection._nbEntries += nbEntriesDelta;
	this.data._nbEntries += nbEntriesDelta;

	delete dataId._fetching;
	this.event.trigger("updated");
}

function yieldThread()
{
	return new Promise((resolve) => {
		setTimeout(() => { resolve(); }, 1);
	});
}

async function garbageCollector()
{
	// If a cleanup is required
	try {
		while (this.data._size > this.config.maxSize || (this.config.maxEntries && this.data._nbEntries > this.config.maxEntries)) {

			// Look for the oldest resource
			const entry = Object.entries(this.resourceMap).reduce((total, cur) => {
				if (!total || cur[1] < total[1]) return cur;
				return total;
			}, null);
			Exception.assert(entry, "Could not find entry to remove");

			Log.info("GARBAGE COLLECTION size=" + this.data._size + ", nbEntries=" + this.data._nbEntries + ": removing entry \"" + entry[0] + "\"");

			// Delete the resource
			deleteResourceById.call(this, entry[0]);

			// Yield
			await yieldThread();
		}
	}
	catch (e) {
		Exception.fromError(e).print();
	}
	finally {
		this.interval = setTimeout(() => {
			garbageCollector.call(this);
		}, this.config.garbageCollectorPeriodMs);
	}
}
