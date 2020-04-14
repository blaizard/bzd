"use strict";

const Log = require("../log.js")("persistence", "memory");
const Exception = require("../exception.js")("persistence", "memory");
const Event = require("../event.js");

/**
 * In-memory persistence only
 */
module.exports = class PersistenceMemory {
	constructor(options) {

		this.options = Object.assign({
			/**
			 * Initial value of the data if no data is currently present.
			 */
			initial: {},
			/**
			 * Type of operations supported.
			 */
			operations: {
				/**
				 * \brief Set a specific key and its value to the data.
				 */
				set: (data, key, value) => {
					data[key] = value;
				},
				/**
				 * \brief Delete a specific key from the data.
				 */
				delete: (data, key) => {
					delete data[key];
				}
			}

		}, options);

		// Make sure that some of the required options are set
		Exception.assert(typeof this.options.initial !== "undefined");
		Exception.assert(typeof this.options.operations === "object");

		// Initialize local variables
		this.isReady = false;
		this.event = new Event({
			ready: {proactive: true}
		});
	}

	/**
	 * \brief Register an event
	 */
	on (...args) {
		this.event.on(...args);
	}

	/**
	 * \brief Initialize the data by reading the content.
	 */
	async initialize() {
		return this.reset();
	}

	/**
	 * \brief Reset the entire persistence to the inital value or a predefined value.
	 */
	async reset(data) {
		// Use the inital value if data is unset
		const setData = (typeof data === "undefined") ? this.options.initial : data;

		// Initialize the data
		this.data = Object.assign({}, setData);
		this.isReady = true;
		this.event.trigger("ready");
	}

	/**
	 * \brief Close the persistence
	 */
	close() {
		// Nothing to do
	}

	/**
	 * This function waits until the persistence is ready
	 */
	async waitReady() {
		return this.event.waitUntil("ready");
	}

	/**
	 * Check if persistence is ready
	 */
	isReady() {
		return this.event.is("ready");
	}

	/**
	 * Wait until all previous operations are completed
	 */
	async waitSync() {
	}

	/**
	 * \brief Return the latest version of the data persisted.
	 */
	get() {
		Exception.assert(this.isReady, "Persistence is not ready yet.");
		return this.data;
	}

	/**
	 * Write new data to the disk
	 *
	 * \param type The type of operation. It can be one of the following:
	 */
	async write(type, ...args) {
		Exception.assert(this.isReady, "Persistence is not ready yet.");
		Exception.assert(this.options.operations.hasOwnProperty(type), "The operation \"" + type + "\" is not supported.");

		// Update the current object
		await this.options.operations[type].call(this.options.operations, this.data, ...args);
	}

	/**
	 * \biref Tells if the perisitence is dirty (hence if it might need a savepoint)
	 */
	isDirty() {
		Exception.assert(this.isReady, "Persistence is not ready yet.");
		return false;
	}

	/**
	 * \brief Write the latest version of the data.
	 */
	async savepoint() {
		Exception.assert(this.isReady, "Persistence is not ready yet.");
	}

	/**
	 * This function is for testing only. It ensures that
	 * the content is consistent.
	 */
	async consistencyCheck() {
		return true;
	}
};
