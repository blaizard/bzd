import PersistenceTimeseries from "../../core/persistence/timeseries.mjs";
import Timeseries from "./timeseries.mjs";
import FileSystem from "../../core/filesystem.mjs";
import Cache from "../../core/cache.mjs";
import Path from "path";
import { CollectionPaging } from "../utils.mjs";

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
			options
		);
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
				Object.assign(
					{
						/**
						 * \brief Perform a savepoint every X seconds
						 */
						savepointIntervalS: 5 * 60,
					},
					bucket in this.options.buckets ? this.options.buckets[bucket] : {}
				);

				// Load the persistence
				const options = {
					savepointTask: null /*{
						namespace: "db::timeseries",
						name: bucket,
						intervalMs: optionsBucket.savepointIntervalS * 1000
					}*/,
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
	 * List the last N entries.
	 *
	 * \param maxOrPaging Paging information.
	 */
	async list(bucket, maxOrPaging) {
		let persistence = await this._getPersistence(bucket);

		// Gather all the data
		let items = [];
		await persistence.waitReady();
		await persistence.forEach((t, data) => {
			items.push([t, data]);
		});
		items.reverse();

		return CollectionPaging.makeFromList(items, maxOrPaging);
	}

	/**
	 * List the last entries until a specific timestamp.
	 *
	 * \param maxOrPaging Paging information.
	 */
	async listUntilTimestamp(bucket, timestamp, maxOrPaging) {
		let persistence = await this._getPersistence(bucket);

		// Gather all the data
		let items = [];
		await persistence.waitReady();
		await persistence.forEach((t, data) => {
			items.push([t, data]);
		}, timestamp);
		items.reverse();

		return CollectionPaging.makeFromList(items, maxOrPaging);
	}
}
