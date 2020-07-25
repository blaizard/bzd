import Event from "../../core/event.mjs";
import ExceptionFactory from "../../core/exception.mjs";

const Exception = ExceptionFactory("db", "timeseries");

/**
 * File storage module
 */
export default class Storage {
	constructor() {
		this.event = new Event({
			ready: { proactive: true },
			error: { proactive: true },
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
	async _initializeImpl() {}

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
	 * Insert a new entry
	 */
	async insert(/*bucket, timestamp, data*/) {
		Exception.unreachable("'insert' must be implemented.");
	}
}
