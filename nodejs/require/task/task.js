"use strict";

const Log = require("../log.js")("task", "task");
const Exception = require("../exception.js")("task", "task");

class TaskId {
	constructor(namespace, name) {
		this.namespace = namespace;
		this.name = name;
	}

	isEqual(taskId) {
		return (taskId.namespace === this.namespace && taskId.name === this.name);
	}

	toString() {
		return this.namespace + "::" + this.name;
	}
};

module.exports = class Task {
	constructor(namespace, name, action, config) {
		this.id = new TaskId(namespace, name);
		this.status = Task.STATUS_IDLE;
		this.action = action;
		this.config = Object.assign({
			/**
			 * Task priority (from 0 to 10), the lower the highest priority
			 */
			priority: Task.PRIORITY_LOW,
			/**
			 * Delay in ms, when the next task should start for the first time
			 */
			delayMs: 0,
			/**
			 * Interval in ms, when the next task should be executed
			 */
			intervalMs: 0,
			/**
			 * Condition that must be satisfied, otherwise the task will be evicted.
			 *
			 * \param iteration The number of times this task has already been executed.
			 */
			isValid: (iteration) => (iteration < 1)
		}, config);
		// The number of times this task has been executed
		this.iteration = 0;
		// The timestamp at which the task will be executed next
		this.timestamp = 0;

		// Sanity check
		Exception.assert(typeof this.config.priority === "number", () => ("config.priority must be a number, instead: " + JSON.stringify(this.config.priority)));
		Exception.assert(this.config.priority >= Task.PRIORITY_LOWEST && this.config.priority <= Task.PRIORITY_HIGHEST, () => ("config.priority is not in the range, instead: " + JSON.stringify(this.config.priority)));
		Exception.assert(typeof this.action === "function", () => ("Task " + this.toString() + " action must be a function, instead: " + JSON.stringify(this.action)));
	}

	/**
	 * Check if a task is equal to this one.
	 *
	 * \param task The task to be compared with this one.
	 *
	 * \return true if equal, fasle otherwise.
	 */
	isEqual(task) {
		return this.id.isEqual(task.id);
	}

	/**
	 * Display a task
	 */
	toString() {
		return String(this.id);
	}

	get isValid() {
		return this.config.isValid(this.iteration);
	}

	get delayMs() {
		return this.config.delayMs || 0;
	}

	get intervalMs() {
		return this.config.intervalMs || 0;
	}

	get priority() {
		return this.config.priority;
	}

	/**
	 * \brief Task priorities
	 * \{
	 */
	static get PRIORITY_LOWEST() { return 0; }
	static get PRIORITY_HIGHEST() { return 3; }
	static get PRIORITY_LOW() { return 0; }
	static get PRIORITY_MEDIUM() { return 1; }
	static get PRIORITY_HIGH() { return 2; }
	static get PRIORITY_CRITICAL() { return 3; }
	/**
	 * \}
	 */

	/**
	 * \brief Task statuses
	 * \{
	 */
	static get STATUS_IDLE() { return "idle"; }
	static get STATUS_PENDING() { return "pending"; }
	/**
	 * \}
	 */
};
