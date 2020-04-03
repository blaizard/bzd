"use strict";

const Log = require("../log.js")("task", "manager");
const Exception = require("../exception.js")("task", "manager");
const Task = require("./task.js");

// Task size approximation, used for to estimate the size of the data structure (in bytes).
const TASK_SIZE = 128; // sizeof(Task);

/**
 * \brief This module is in charge of monitoring all tasks and running them.
 */
module.exports = class Manager {
	constructor(config) {
		this.config = Object.assign({

		}, config);

		this.tasks = {
			size: 0
		};
		Exception.assert(Task.PRIORITY_LOWEST === 0, "This assumes that the lowest priority is 0");
		this.taskQueues = [...Array(Task.PRIORITY_HIGHEST + 1)].map(() => []);

		// Task executor
		this.schedulerTimestamp = 0;
		this.schedulerInstance = null;
		this.schedulerRunning = false;
	}

	/**
	 * Current timestamp
	 */
	static getTimestampMs() {
		return Date.now();
	}

	/**
	 * Register a specific task and associated it with its namespace and task Ids.
	 * A registered task will run when needed.
	 *
	 * \param namespace The namespace identifier to which this task belongs to.
	 * \param name The name of the task (part of the namespace) to which this task belongs to.
	 * \param action A callback describing the action to be performed.
	 * \param config Extra configuration for the task to be resigisterd.
	 *
	 * \return The new task created.
	 */
	register(namespace, name, action, config) {
		Exception.assert(namespace !== "size", "Namespace ID cannot be size");
		Exception.assert(name !== "size", "Task ID cannot be size");

		// Create
		const task = new Task(namespace, name, action, config);

		// If the task is already registered, unregister it
		if (this.isRegistered(task.id)) {
			this.unregister(task.id);
		}

		// Register
		Log.info("Registering task " + task + " with priority " + task.priority);
		this.tasks[task.id.namespace] = this.tasks[task.id.namespace] || {
			size: 0
		};
		this.tasks[task.id.namespace][task.id.name] = task;
		this.tasks[task.id.namespace].size += TASK_SIZE;
		this.tasks.size += TASK_SIZE;

		// Insert to the queue
		this.scheduleTaskIfValid(task, task.delayMs, /*triggerTaskScheduler*/true);

		return task;
	}

	/**
	 * Delete a previously registered task.
	 *
	 * \param taskId The task identifier.
	 */
	unregister(taskId) {
		Exception.assert(this.isRegistered(taskId), "The task " + taskId + " is not registered");

		const task = this.tasks[taskId.namespace][taskId.name];
		Log.info("Unregistering task " + task);

		// If the task is pending (it is in the queue but not executed yet),
		// remove it from the list.
		if (task.status == Task.STATUS_PENDING) {
			const index = Manager.findInQueue(this.taskQueues[task.priority], task);
			Exception.assert(index !== -1, "The task is pending but it cannot be found in the queue");
			this.taskQueues[task.priority].splice(index, 1); 
		}

		// Delete the registered task
		delete this.tasks[taskId.namespace][taskId.name];
		this.tasks[taskId.namespace].size -= TASK_SIZE;
		this.tasks.size -= TASK_SIZE;
	}

	/**
	 * Check if a task is alredy registered.
	 *
	 * \param taskId The task identifier.
	 *
	 * \return true if registered, false otherwise.
	 */
	isRegistered(taskId) {
		return this.tasks.hasOwnProperty(taskId.namespace) && this.tasks[taskId.namespace].hasOwnProperty(taskId.name);
	}

	/**
	 * Insert a task in the task queue, the task should be registered and with idle status.
	 *
	 * \param task The task to be inserted.
	 * \param delayMs The delay in ms after which the task should be scheduled.
	 * \param triggerTaskScheduler Trigger the task scheduler or not.
	 */
	scheduleTaskIfValid(task, delayMs, triggerTaskScheduler) {
		Exception.assert(this.isRegistered(task.id), "The task " + task + " is not registered");
		Exception.assert(task.status === Task.STATUS_IDLE, () => ("Task " + task + " should be have its status to idle, instead: " + task.status));
		Exception.assert(typeof delayMs === "number", () => ("The delay specified is not a number: " + delayMs));

		let isValid = false;
		try {
			isValid = task.isValid;
		}
		catch (e) {
			Exception.fromError(e).print("Task " + task + " validation failed");
		}

		// If the task is valid, add it to the queue
		if (isValid) {

			// Calculate the next execution timestamp
			const timestamp = Manager.getTimestampMs() + delayMs;

			// Add the task to the task queue
			task.status = Task.STATUS_PENDING;
			task.timestamp = timestamp;
			Manager.insertInQueue(this.taskQueues[task.priority], task);

			// Trigger the task scheduler
			if (triggerTaskScheduler) {
				this.triggerTaskScheduler();
			}
		}
		// If not unregister is
		else {
			this.unregister(task.id);
		}
	}

	/**
	 * Get the next task to be executed. The choice takes into consideration the current timestamp
	 * as well as the task priority.
	 *
	 * \return The next task to be executed (the task is removed from the list) or null if no task
	 * needed to be executed now (which does not mean that the task list is empty).
	 */
	popNextTask() {
		const timestamp = Manager.getTimestampMs();

		// Look for the next task which timestamp already past,
		// starting from the highest priority
		for (let priority = this.taskQueues.length - 1; priority >= 0; --priority) {
			if (this.taskQueues[priority].length && this.taskQueues[priority][0].timestamp <= timestamp) {
				return this.taskQueues[priority].shift();
			}
		}

		// No task was found
		return null;
	}

	/**
	 * Get the next timestamp of a task to be executed.
	 *
	 * \return the timestamp in ms of the next task to be executed or null if the task list is empty.
	 */
	getNextTaskTimestamp() {
		const minTimestamp = this.taskQueues.reduce((minTimestamp, queue) => {
			return (queue.length) ? Math.min(queue[0].timestamp, minTimestamp) : minTimestamp;
		}, Number.MAX_VALUE);
		return (minTimestamp == Number.MAX_VALUE) ? null : minTimestamp;
	}

	/**
	 * Trigger the execution of the scheduler.
	 */
	triggerTaskScheduler() {

		// If the scheduler is already running, do nothing
		if (this.schedulerRunning) {
			return;
		}

		const timestamp = Manager.getTimestampMs();
		const nextTimestamp = this.getNextTaskTimestamp();

		if (nextTimestamp !== null) {
			// Task scheduler is running and its next execution is too late,
			// stop it and restart it
			if (this.schedulerInstance && nextTimestamp < this.schedulerTimestamp) {
				clearTimeout(this.schedulerInstance);
				this.schedulerInstance = null;
			}

			// Task scheduler is not running, start it
			if (!this.schedulerInstance) {
				this.schedulerInstance = setTimeout(() => { this.taskScheduler() }, (nextTimestamp > timestamp) ? nextTimestamp - timestamp : 1);
				this.schedulerTimestamp = nextTimestamp;
			}
		}
	}

	/**
	 * Run current tasks and trigger the next one
	 */
	async taskScheduler() {

		// This should never happen, otherwise somethign is wrong in the logic
		Exception.assert(this.schedulerRunning === false, "Task scheduler is already running");
		this.schedulerRunning = true;

		try {
			// Execute the next tasks if any
			let task = null;
			while (task = this.popNextTask()) {

				try {
					if (task.isValid) {
						++task.iteration;
						await task.action();
					}
				}
				catch (e) {
					Exception.fromError(e).print("Task " + task + " execution error");
				}
				finally {
					task.status = Task.STATUS_IDLE
				}

				// Re-insert the task if valid. Note, if the task is invalid
				// it will not insert the task but unregister it
				this.scheduleTaskIfValid(task, task.intervalMs, /*triggerTaskScheduler*/false);
			}
		}
		finally {
			// Mark the scheduler as not running anymore
			this.schedulerInstance = null;
			this.schedulerRunning = false;
		}

		// Re-trigger the scheduler
		this.triggerTaskScheduler();
	}

	/**
	 * Return in json format the current state of the task manager
	 */
	toJson(namespace) {
		const timestamp = Manager.getTimestampMs();
		return Object.keys(this.tasks[namespace]).filter((name) => (name != "size")).map((name) => {
			const task = this.tasks[namespace][name];
			return {
				name: name,
				status: task.status,
				priority: task.priority,
				timestampDelta: Math.max(0, task.timestamp - timestamp)
			}
		});
	}

	/**
	 * Print a human readable representation of the task list
	 */
	toString() {
		const outputList = [];
		for (const namespace in this.tasks) {
			if (namespace != "size") {
				for (const name in this.tasks[namespace]) {
					if (name != "size") {
						const task = this.tasks[namespace][name];
						outputList.push(task + " (" + task.status + ")");
					}
				}
			}
		}
		return outputList.join("\n");
	}

	/**
	 * Find a specific task in the queue.
	 *
	 * \param queue The queue to look into.
	 * \param task The task to be looked for.
	 *
	 * \return the index of the task in the queue if found, -1 otherwise.
	 */
	static findInQueue(queue, task) {

		let minIndex = 0;
		let maxIndex = queue.length - 1;
		let currentIndex = 0;

		// Look for the entry
		while (minIndex <= maxIndex) {
			// | 0 is equivalent to Math.floor
			currentIndex = (minIndex + maxIndex) / 2 | 0;

			if (queue[currentIndex].timestamp < task.timestamp) {
				minIndex = currentIndex + 1;
			}
			else if (queue[currentIndex].timestamp > task.timestamp) {
				maxIndex = currentIndex - 1;
			}
			// Found exact match. From there look for the exact task
			else {
				for (let i = currentIndex; i >= 0 && queue[i].timestamp == task.timestamp; --i) {
					if (task.isEqual(queue[i])) {
						return i;
					}
				}
				for (let i = currentIndex + 1; i < queue.length && queue[i].timestamp == task.timestamp; ++i) {
					if (task.isEqual(queue[i])) {
						return i;
					}
				}
				break;
			}
		}

		return -1;
	}

	/**
	 * Insert in a sorted queue
	 */
	static insertInQueue(queue, task) {

		let minIndex = 0;
		let maxIndex = queue.length - 1;
		let currentIndex = 0;

		// Look for the entry
		while (minIndex <= maxIndex) {
			// | 0 is equivalent to Math.floor
			currentIndex = (minIndex + maxIndex) / 2 | 0;

			if (queue[currentIndex].timestamp < task.timestamp) {
				minIndex = currentIndex + 1;
			}
			else if (queue[currentIndex].timestamp > task.timestamp) {
				maxIndex = currentIndex - 1;
			}
			// Found value equal
			else {
				break;
			}
		}

		// Insert
		currentIndex = (queue[currentIndex] && queue[currentIndex].timestamp < task.timestamp) ? currentIndex + 1 : currentIndex;
		queue.splice(currentIndex, 0, task);
	}
}
