import Path from "path";

import Cache2 from "../../core/cache2.js";
import FileSystem from "../../core/filesystem.js";
import LogFactory from "../../core/log.js";
import PersistenceTimeseries from "../../core/persistence/timeseries.js";
import { CollectionPaging } from "../utils.js";

import Timeseries from "./timeseries.js";

const Log = LogFactory("db", "timeseries", "disk");

/**
 * Disk timeseries module
 */
export default class TimeseriesDisk extends Timeseries {
	constructor(path, options) {
		super();

		this.options = Object.assign(
			{
				buckets: {},
			},
			options,
		);
		// The path where to store the database
		this.path = path;
		// Contains all buckets registered in the cache
		this.registeredBuckets = new Set();

		this.cache = null;

		Log.info("Using disk timeseries DB at '{}'.", this.path);
	}

	/**
	 * Initialize the timeseries module
	 */
	async _initialize() {
		// Create the directory if it does not exists
		await FileSystem.mkdir(this.path);
		this.cache = new Cache2("timeseries-disk-cache");
	}

	/**
	 * Return the persistence associated with a specific bucket and, if needed, create it and load it.
	 */
	async _getPersistence(bucket) {
		if (!this.registeredBuckets.has(bucket)) {
			this.registeredBuckets.add(bucket);
			// Register this bucket in the cache
			this.cache.register(
				bucket,
				async () => {
					// Read bucket specific options
					Object.assign(
						{
							/**
							 * \brief Perform a savepoint every X seconds
							 */
							savepointIntervalS: 5 * 60,
						},
						bucket in this.options.buckets ? this.options.buckets[bucket] : {},
					);

					// Load the persistence
					const options = {
						savepointTask: null /*{
	   namespace: "db::timeseries",
	   name: bucket,
	   intervalMs: optionsBucket.savepointIntervalS * 1000
}*/,
					};

					let persistence = await PersistenceTimeseries.make(Path.join(this.path, bucket), options);
					return persistence;
				},
				// Persistencies are kept open until the module is shut down, they should never expire.
				{ timeoutMs: Infinity },
			);
		}

		let persistence = await this.cache.get(bucket, "default");
		return persistence;
	}

	async _insertImpl(bucket, timestamp, data) {
		let persistence = await this._getPersistence(bucket);
		await persistence.insert(timestamp, data);
	}

	/**
	 * List the last N entries.
	 *
	 * \param maxOrPaging Paging information.
	 */
	async _listImpl(bucket, maxOrPaging) {
		let persistence = await this._getPersistence(bucket);

		// Gather all the data
		let items = [];
		await persistence.waitReady();
		await persistence.forEach((t, data) => {
			items.push([t, data]);
		});
		items.reverse();

		return await CollectionPaging.makeFromList(items, maxOrPaging);
	}

	/**
	 * List the last entries until a specific timestamp.
	 *
	 * \param maxOrPaging Paging information.
	 */
	async _listUntilTimestampImpl(bucket, timestamp, maxOrPaging) {
		let persistence = await this._getPersistence(bucket);

		// Gather all the data
		let items = [];
		await persistence.waitReady();
		await persistence.forEach((t, data) => {
			items.push([t, data]);
		}, timestamp);
		items.reverse();

		return await CollectionPaging.makeFromList(items, maxOrPaging);
	}
}
