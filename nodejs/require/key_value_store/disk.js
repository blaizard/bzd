"use strict";

const Path = require("path");

const Log = require("../log.js")("key-value", "disk");
const Exception = require("../exception.js")("key-value", "disk");
const PersistenceDisk = require("../persistence/disk.js");
const Cache = require("../cache.js");
const Event = require("../event.js");
const FileSystem = require("../filesystem.js");

/**
 * Key value store for low demanding application, that presists on the local disk.
 */
module.exports = class KeyValueStoreDisk {

	constructor(path, options) {
        this.options = Object.assign({
            /**
             * Bucket specific options 
             */
			buckets: {},
		}, options);
        // The path where to store the database
        this.path = path;
        // Contains all open persistencies (aka buckets)
        this.persistences = {};

        this.cache = null;
        this.event = new Event({
            ready: {proactive: true}
        });

        // Initialize ansynchronously the rest of the data
        this._initAsync();
    }

    /**
	 * Initialization of the class
	 */
	async _initAsync() {

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
                    initialize: true,
                    savepointTask: {
                        namespace: "kvs",
                        name: bucket,
                        intervalMs: optionsBucket.savepointIntervalS * 1000
                    }
                };

                let persistence = new PersistenceDisk(Path.join(this.path, bucket), options);
                await persistence.waitReady();
                // Preload the data in order to get accurate attribute size 
                await persistence.get();
                return persistence;
            });
        }

        let persistence = await this.cache.get(bucket);
        return persistence;
    }

	/**
	 * This function waits until the key value store database is ready
	 */
	async waitReady() {
		return this.event.waitUntil("ready");
	}

    async set(bucket, key, value) {
        let persistence = await this._getPersistence(bucket);
        await persistence.write("set", key, value);
    }

    async get(bucket, key, defaultValue = undefined) {
        let persistence = await this._getPersistence(bucket);
        const data = await persistence.get();
        return (key in data) ? data[key] : defaultValue;
    }

    async delete(bucket, key) {
        let persistence = await this._getPersistence(bucket);
        await persistence.write("delete", key);
    }
};
