"use strict";

/**
 * Simple mutex implementation
 */
module.exports = class Mutex {
	constructor() {
		this.queue = [];
		this.isLock = false;
	}

	/**
	 * Lock the ressource and call the callback once the ressource is under the lock.
	 */
	lock(callback) {

		// Use promises if the callback is not set
		if (typeof callback === "undefined") {
			return new Promise((resolve) => {
				this.lock(resolve);
			});
		}

		// If the lock is already taken, add the current process to the queue
		// This is done atomically, as a Javascript operation
		if (this.isLock) {
			this.queue.push(callback);
			return;
		}

		// Otherwise acquire the lock
		this.isLock = true;
		callback(() => {
			this.release();
		});
	}

	/**
	 * Release the current lock and execute (if any) the next process
	 */
	release() {
		// If there are pending lock in the queue
		if (this.queue.length > 0) {
			const callback = this.queue.shift();
			// Immediatly process the next item with the next tick. This is done in order
			// to avoid long call stacks.
			process.nextTick(() => {
				callback(() => {
					this.release();
				});
			});
		}
		else {
			this.isLock = false;
		}
	}
};
