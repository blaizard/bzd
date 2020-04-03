"use strict";

const Path = require("path");
const Log = require("../log.js")("persistence", "timeseries");
const Exception = require("../exception.js")("persistence", "timeseries");
const Event = require("../event.js");
const FileSystem = require("../filesystem.js");
const Persistence = require("./disk.js");
const TimeSeries = require("../timeseries.js");

const VERSION = 2;

/**
 * Persist a timeseries data structure
 */
module.exports = class PersistenceTimeSeries {

	/**
	 * \param path Directory where to store the timeseries
	 */
	constructor(path, options) {
		this.options = Object.assign({
			unique: false,
			uniqueMerge: (a, /*b, timestamp*/) => a,
			/**
			 * The directory name where the actual data should be stored.
			 * This directory is relative to the file being opened.
			 */
			dataDir: "%s-data",
			/**
			 * Default periodic savepoint task
			 */
			savepointTask: {
				intervalMs: 5000 * 60 // 5min by default
			},
			/**
			 * Maximum number of entries before spliting files
			 */
			maxEntriesPerFile: 100
		}, options);

		this.event = new Event({
			ready: {proactive: true}
		});

		const filePath = Path.basename(path);
		const dirPath = Path.dirname(path)

		// Create the dataDir
		this.options.dataDir = Path.join(dirPath, this.options.dataDir.replace("%s", filePath));

		// Where all open data persistences are stored
		this.persistenceCache = {};
		this.persistenceIndex = null;
		this.timeSeriesIndex = new TimeSeries({
			unique: true,
			uniqueMerge: (a, b, timestamp) => {
				Exception.unreachable("Duplicate timestamps (" + timestamp + ") are in the index for entries: " + a + " and " + b);
			}
		});
		this.timeSeriesData = new TimeSeries({
			unique: this.options.unique,
			uniqueMerge: this.options.uniqueMerge
		});

		this.initialize(path);
	}

	/**
	 * Initialize the persistence
	 */
	async initialize(path) {

		// Load the index
		this.persistenceIndex = new Persistence(path, this.optionsIndex);
		await this.persistenceIndex.waitReady();
		// Quick check to see if the index need to be rebuilt
		// There seems to be an issue if it is not rebuilt (test fails sporadically)
		if ((await this.persistenceIndex.get()).version != VERSION || true) {
			await this.rebuildIndex();
			Log.info("Successfully rebuilt index of timeseries " + path);
		}
		this.event.trigger("ready");
	}

	/**
	 * Close the persistence
	 */
	async close() {
		let promiseList = [];
		promiseList.push(this.persistenceIndex.close());
		for (const id in this.persistenceCache) {
			promiseList.push(this.persistenceCache[id].close());
		}
		await Promise.all(promiseList);
	}

	/**
	 * Insert a new data point
	 */
	async insert(timestamp, data) {
		let result = await this.getPersistenceByTimestamp(timestamp, /*forWrite*/true);
		const promiseData = result.persistence.write("insert", timestamp, data);
		const promiseIndex = this.persistenceIndex.write("update", timestamp);
		await Promise.all([promiseData, promiseIndex]);
	}

	/**
	 * Loop through the data.
	 * 
	 * \param callback The function to be called for each entry.
	 * \param timestampStart The starting timestamp. If omitted, it will start at the begining
	 * \param timestampEnd The ending timestamp. If omitted, it will go until the end.
	 * \param inclusive Used only if the timestamps do not match anything. If set to true, previous
	 * and next timestamp will be included. Otherwise not.
	 * 
	 * \todo support for each over files
	 */
	async forEach(callback, timestampStart, timestampEnd, inclusive) {

		const firstValidTimestamp = await this.getTimestamp(0);

		// Means that there is no sample
		if (firstValidTimestamp === null) {
			return;
		}

		// Make sure the first timestamps is valid
		let timestamp = (typeof timestampStart === "undefined") ? -Number.MAX_VALUE : timestampStart;
		timestamp = Math.max(timestamp, await this.getTimestamp(0));
		timestampEnd = (typeof timestampEnd === "undefined") ? (await this.getTimestamp(-1)) : timestampEnd;

		// Loop through the entries
		let isFirst = true;
		do {
			const result = await this.getPersistenceByTimestamp(timestamp, /*forWrite*/false, (isFirst) ? TimeSeries.FIND_IMMEDIATELY_BEFORE : TimeSeries.FIND_IMMEDIATELY_AFTER);
			
			isFirst = false;
			const lastValidTimestamp = await this.timeSeriesData.wrap((await result.persistence.get()).list, (timeseries) => {
				timeseries.forEach(callback, timestamp, timestampEnd, inclusive);
				return timeseries.getTimestamp(-1);
			});
			// Check if it needs to continue.
			// Note +1 is important as it ensure that the timestamp is not exsiting in the current list, hence
			// avoid an infinity loop situation.
			timestamp = (timestampEnd > lastValidTimestamp) ? (lastValidTimestamp + 1) : null;
		} while (timestamp != null);
	}

	/**
	 * Get a known timestamp value.
	 * 
	 * \param index A positive value will return the timestamp stating from the begning
	 * with an offset eequal to the index. A negative value, will return the timestamp
	 * from the end.
	 * For example, 0 will return the oldest timestamp while -1 will return the newest.
	 * 
	 * \return The timestamp or null if out of bound.
	 */
	async getTimestamp(index) {
		let result = await this.getPersistenceByIndex(index);
		// Out of bound
		if (result === null) {
			return null;
		}
		return await this.timeSeriesData.wrap((await result.persistence.get()).list, (timeseries) => {
			return timeseries.getTimestamp(result.index);
		});
	}

	/**
	 * This function waits until the persistence is ready
	 */
	async waitReady() {
		await this.persistenceIndex.waitReady();
		return this.event.waitUntil("ready");
	}

	// ---- private methods ---------------------------------------------------

	/**
	 * Rebuild the index in case of corruption, or migration.
	 */
	async rebuildIndex() {
		Log.info("Re-building index, this might take a while...");
		// List all data files
		const fileList = (await FileSystem.exists(this.options.dataDir)) ? await FileSystem.readdir(this.options.dataDir) : [];

		// Create the list of data file
		let dataFileList = new Set();
		for (const i in fileList) {
			const fullPath = Path.join(this.options.dataDir, fileList[i]);
			const stat = await FileSystem.stat(fullPath);
			if (stat.isFile()) {
				dataFileList.add(fileList[i]);
			}
			else if (stat.isDirectory() && fileList[i].match(/^.+\.delta$/)) {
				dataFileList.add(fileList[i].replace(/\.delta$/, ""));
			}
		}
		dataFileList = Array.from(dataFileList);

		Log.info("Identified " + dataFileList.length + " data file(s)");

		// Get the metadata out of them
		let timeseriesData = [];
		await this.timeSeriesIndex.wrap(timeseriesData, async (timeSeriesIndex) => {

			for (const i in dataFileList) {
				const fullPath = Path.join(this.options.dataDir, dataFileList[i]);
				let persistence = null;
				try {
					// Open the persistence
					persistence = await new Persistence(fullPath, this.optionsDataReadOnly);
					await persistence.waitReady();
					// Read some metadata
					await this.timeSeriesData.wrap((await persistence.get()).list, (timeSeriesData) => {

						Exception.assert(timeSeriesData.consistencyCheck(), "Data file '" + fullPath + "' is not consistent");

						const timestampBegin = timeSeriesData.getTimestamp(0);
						const timestampEnd = timeSeriesData.getTimestamp(-1);
						const timestampLength = timeSeriesData.length;

						// Inset the data
						timeSeriesIndex.insert(timestampBegin, {
							path: dataFileList[i],
							length: timestampLength,
							timestampEnd: timestampEnd
						});
					});
				}
				catch (e) {
					Log.error("Ignoring data file '" + fullPath + "' seems to be corrupted: " + e);
				}
				finally {
					// Close the persistence
					await persistence.close();
				}
			}
		});

		// Build index full content
		const indexContent = {
			version: VERSION,
			list: timeseriesData
		};
		// Reset the index persistence with this content
		await this.persistenceIndex.reset(indexContent);
		await this.persistenceIndex.waitReady();
	}

	get optionsIndex() {
		return {
			initial: {list:[]},
			operations: {
				insert: async (data, timestamp, value) => {
					await this.timeSeriesIndex.wrap(data.list, (t) => {
						t.insert(timestamp, value);
					});
				},
				update: async (data, timestamp) => {
					await this.timeSeriesIndex.wrap(data.list, (t) => {
						const index = t.find(timestamp, TimeSeries.FIND_IMMEDIATELY_BEFORE);

						// Increase the length
						Exception.assert(typeof t.data[index] == "object", () => ("Cannot find timestamp " + timestamp + " in index, returned index: " + index + " " + JSON.stringify(t.data)));
						Exception.assert(t.data[index][1].hasOwnProperty("length"), "No length property for index entry " + index);
						++(t.data[index][1].length);

						// Update the timestampEnd field
						Exception.assert(t.data[index][1].hasOwnProperty("timestampEnd"), () => ("No timestampEnd property for index entry " + index + "  " + JSON.stringify(t.data[index])));
						t.data[index][1].timestampEnd = Math.max(t.data[index][1].timestampEnd, timestamp);
					});
				}
			},
			savepointTask: this.options.savepointTask
		};
	}

	get optionsDataReadOnly() {
		return {
			initial: {list:[]},
			operations: {
				insert: async (data, timestamp, value) => {
					await this.timeSeriesData.wrap(data.list, (t) => {
						t.insert(timestamp, value);
					});
				}
			}
		};
	}

	get optionsData() {
		return Object.assign({
			savepointTask: this.options.savepointTask
		}, this.optionsDataReadOnly);
	}

	/**
	 * Return and create if necessary the persistence associated with this timestamp
	 * 
	 * \param timestamp The timestamp associated with the persistence to be opened
	 * \param forWrite Optional, Open the persistence for write (or create one if needed)
	 * \param mode The search mode if no exact match.
	 * 
	 * \return A structure containing the persistence and the metadata;
	 */
	async getPersistenceByTimestamp(timestamp, forWrite = false, mode = TimeSeries.FIND_IMMEDIATELY_BEFORE) {
		Exception.assert(typeof timestamp == "number", "Timestamp passed to getPersistenceByTimestamp is not a number: " + timestamp);

		const metadata = await this.getMetadata(timestamp, forWrite, mode);
		Exception.assert(metadata, "Metadata returned for " + timestamp + " is evaluating to false");

		// Load the persistence if not alreay loaded
		if (!this.persistenceCache.hasOwnProperty(metadata.path)) {
			Log.info("Loading time series from " + metadata.path);

			await FileSystem.mkdir(this.options.dataDir);
			const fullPath = Path.join(this.options.dataDir, metadata.path);
			let persistence = new Persistence(fullPath, this.optionsData);
			await persistence.waitReady();

			this.persistenceCache[metadata.path] = persistence;
		}

		Exception.assert(this.persistenceCache.hasOwnProperty(metadata.path), "Persistence " + metadata.path + " does not exists");
		Exception.assert(this.persistenceCache[metadata.path] instanceof Persistence, "Persistence " + metadata.path + " is not of Persistence type");
		Exception.assert(this.persistenceCache[metadata.path].isReady(), "Persistence " + metadata.path + " is not ready");

		return Object.assign({
			persistence: this.persistenceCache[metadata.path]
		}, metadata);
	}

	/**
	 * \brief Get the persistence from a specific index
	 * 
	 * \return A structure containing the persistence and the relative index
	 * to this persistence or null if out of bound.
	 */
	async getPersistenceByIndex(index) {

		let result = null;
		if (index >= 0) {
			// Loop through the index and count the element until index is reached
			result = await this.timeSeriesIndex.wrap((await this.persistenceIndex.get()).list, async (timeSeriesIndex) => {
				let indexLeft = index;
				for (let i = 0; i < timeSeriesIndex.data.length; ++i) {
					const entry = timeSeriesIndex.data[i];
					if (indexLeft <= entry[1].length) {
						return {
							timestamp: entry[0],
							index: indexLeft
						}
					}
					indexLeft -= entry[1].length;
				}
			});
		}
		else {
			// Loop through the index and count the element until index is reached
			result = await this.timeSeriesIndex.wrap((await this.persistenceIndex.get()).list, async (timeSeriesIndex) => {
				let indexLeft = -(index + 1);
				for (let i = timeSeriesIndex.data.length - 1; i >= 0; --i) {
					const entry = timeSeriesIndex.data[i];
					if (indexLeft <= entry[1].length) {
						return {
							timestamp: entry[0],
							index: -indexLeft - 1
						}
					}
					indexLeft -= entry[1].length;
				}
			});
		}

		// Out of bound
		if (result === undefined) {
			return null;
		}
		return Object.assign({
			index: result.index
		}, await this.getPersistenceByTimestamp(result.timestamp, /*forWrite*/false));
	}

	/**
	 * Create a new entry in the index file and return its metadata
	 */
	async createIndexEntry(timestamp) {
		const metadata = {
			length: 0,
			path: timestamp + ".json",
			timestampEnd: 0
		};
		await this.persistenceIndex.write("insert", timestamp, metadata);
		return metadata;
	}

	/**
	 * Get the current data file path for this timestamp
	 * 
	 * If no file exists, create a new if 'forWrite' is set.
	 * 
	 * \param timestamp The timestamp associated with the metadata the user wants to acquired.
	 * \param forWrite If set, it will create a file if needed. If not set, it needs to ensure that the timestamp is valid.
	 * \param mode The search mode if no exact match.
	 * 
	 * \return The metadata including the timestamp of the file.
	 */
	async getMetadata(timestamp, forWrite, mode = TimeSeries.FIND_IMMEDIATELY_BEFORE) {
		return await this.timeSeriesIndex.wrap((await this.persistenceIndex.get()).list, async (timeSeriesIndex) => {
			// Look for the entry
			let index = timeSeriesIndex.find(timestamp, mode);
			let metadata = null;

			if (index != -1) {
				metadata = timeSeriesIndex.data[index][1];
				// If the index is found and the index is already full and the requested timestamp should be placed at the end of the file,
				// then create a new file
				if (forWrite && metadata.length >= this.options.maxEntriesPerFile && metadata.timestampEnd < timestamp) {
					Log.info("Creating a new data file, current is full");
					index = -1;
				}
				else {
					timestamp = timeSeriesIndex.data[index][0];
				}
			}

			// No entries, then create one
			if (index == -1) {
				Exception.assert(forWrite, "File does not exists for timestamp " + timestamp + " and not able to create one");
				metadata = await this.createIndexEntry(timestamp);
			}

			return {
				// The timestamp of the file (the one that can be retreived by the index)
				timestamp: timestamp,
				// The path of the data file
				path: metadata.path
			}
		});
	}
}
