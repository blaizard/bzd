"use strict";

import PersistenceTimeseries from "../../core/persistence/timeseries.mjs";
import Timeseries from "./timeseries.mjs";
import FileSystem from "../../core/filesystem.mjs";
import Cache from "../../core/cache.mjs";
import Path from "path";

/**
 * Disk timeseries module
 */
export default class TimeseriesDisk extends Timeseries {

	constructor(path, options) {
		super();

		this.options = Object.assign({
			buckets: {}
		}, options);
		// The path where to store the database
		this.path = path;
		// Contains all open persistencies (aka buckets)
		this.persistences = {};

		this.cache = null;

		this._initialize();
	}

	/**
     * Initialize the timeseries module
     */
	async _initializeImpl() {

		// Create the directory if it does not exists
		await FileSystem.mkdir(this.path);
		this.cache = new Cache();
		this.event.trigger("ready");
	}

	/**
     * Return the persistence associated with a specific bucket and, if needed, create it and load it.
     */
	async _getPersistence(bucket) {

		if (!this.cache.isCollection(bucket)) {

			// Register this bucket in the cache
			this.cache.register(bucket, async () => {

				// Read bucket specific options
				const optionsBucket = Object.assign({
					/**
                     * \brief Perform a savepoint every X seconds
                     */
					savepointIntervalS: 5 * 60
				}, (bucket in this.options.buckets) ? this.options.buckets[bucket] : {});

				// Load the persistence
				const options = {
					savepointTask: null/*{
						namespace: "db::timeseries",
						name: bucket,
						intervalMs: optionsBucket.savepointIntervalS * 1000
					}*/
				};

				let persistence = new PersistenceTimeseries(Path.join(this.path, bucket), options);
				await persistence.waitReady();
				return persistence;
			});
		}

		let persistence = await this.cache.get(bucket);
		return persistence;
	}

	async insert(bucket, timestamp, data) {
		let persistence = await this._getPersistence(bucket);
		await persistence.insert(timestamp, data);
	}

	/**
	 * Retrieve the 'maxEntries' most recent entries from 'timestamp'
	 * 
	 * This function needs to be optimized
	 */
	async get(bucket, timestamp, maxEntries) {
		let persistence = await this._getPersistence(bucket);
		let items = [];
		await persistence.waitReady();
		await persistence.forEach((timestamp, data) => {
			items.push([timestamp, data]);
		});
		const extract = items.slice(-maxEntries);
		extract.reverse();
		return extract;
	}
}
