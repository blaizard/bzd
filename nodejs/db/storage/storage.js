"use strict";

import Event from "../../core/event.js";
import LogFactory from "../../core/log.js";
import ExceptionFactory from "../../core/exception.js";

const Log = LogFactory("db", "storage");
const Exception = ExceptionFactory("db", "storage");

/**
 * File storage module
 */
export default class Storage {

	constructor() {
        this.event = new Event({
            ready: {proactive: true},
            error: {proactive: true}
        });
    }

    /**
     * Initialize the storage module
     */
    async _initialize() {
        try {
            this.event.trigger("ready");
            await this._initializeImpl();
        }
        catch (e) {
            this.event.trigger("error", e);
            throw e;
        }
    }

    /**
     * Initialization function (optional)
     */
    async _initializeImpl() {
    }

	/**
	 * This function waits until the persistence is ready
	 */
	async waitReady() {
		return await this.event.waitUntil("ready");
	}

	/**
	 * Check if persistence is ready
	 */
	isReady() {
		return this.event.is("ready");
	}

    /**
     * Tell whether a key exists or not
     */
    async is(/*bucket, key*/) {
        Exception.unreachable("'is' must be implemented.");
    }

    /**
     * Return a file read stream from a specific key
     */
    async read(/*bucket, key*/) {
        Exception.unreachable("'read' must be implemented.");
    }

    /**
     * Store a file to a specific key
     */
    async write(/*bucket, key, data*/) {
        Exception.unreachable("'write' must be implemented.");
    }
}
