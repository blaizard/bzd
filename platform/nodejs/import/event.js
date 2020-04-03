'use strict';

/**
 * Add events functionality
 */
export default class Event {
	constructor(options) {

		// Loop through event options and adjust them
		this.options = {};
		for (const id in options || {}) {
			this.options[id] = Object.assign({
				/**
				 * Set to true if the event once triggered will also
				 * trigger new one.
				 */
				proactive: false
			}, options[id]);
		}

		this.clear();
	}

	/**
	 * \brief Register an event
	 *
	 * \param id The identifier of the event
	 * \param callback The function to be called when the event is triggered
	 * \param once If the event is intended to be fired only once
	 */
	on(id, callback, once) {
		let addToList = true;

		// Handle proactive events
		if (this.options[id] && this.options[id].proactive && typeof this.proactive[id] !== "undefined") {
			callback.apply(this, this.proactive[id]);
			addToList = !once;
		}

		if (addToList) {
			this.list.hasOwnProperty(id) || (this.list[id] = []);
			this.list[id].push([callback, once]);
		}
		return this;
	}

	/**
	 * \brief Check if an event is active (only for proactive events)
	 * 
	 * \param id The identifier of the events to be triggered
	 * 
	 * \return true if the evetn has been triggered, false otherwise.
	 */
	is (id) {
		return (typeof this.proactive[id] !== "undefined");
	}

	/**
	 * \brief Trigger all events associated with a specific id
	 *
	 * \param id The identifier of the events to be triggered
	 * \param ... Arguments to be passed to the event callbacks
	 */
	trigger (id, ...args) {
		if (this.list[id]) {
			this.list[id].forEach((e) => {
				e[0].apply(this, args);
			});
			this.list[id] = this.list[id].filter((e) => {
				return (e[1] !== true);
			});
		}

		// If this is a proactive event, save the call context for later
		if (this.options[id] && this.options[id].proactive) {
			this.proactive[id] = args || [];
		}
		return this;
	}

	/**
	 * \brief Clear a specific event or all events.
	 *
	 * Clearing events will remove the proactive trigger for example.
	 * It will also remove all previously attached events.
	 */
	clear (id, onlyProactive) {
		if (typeof id === "undefined") {
			this.proactive = {};
			this.list = {};
		}
		else {
			delete this.proactive[id];
			if (!onlyProactive) {
				delete this.list[id];
			}
		}
		return this;
	}

	/**
	 * Wait until the specific id is triggered
	 */
	async waitUntil(id) {
		return new Promise((resolve) => {
			this.on(id, () => {
				resolve();
			}, /*once*/true);
		});
	}
}
