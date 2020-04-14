"use strict";

const Log = require("../log.js")("persistence", "disk");
const Exception = require("../exception.js")("persistence", "disk");
const Event = require("../event.js");
const FileSystem = require("../filesystem.js");
const Mutex = require("../mutex.js");
const TaskManager = require("../task/manager-singleton.js");

/**
 * This module is responsible for keeping the data consistent and persistent.
 * It acts as a on-memory database and save the content recurrently to the disk, to ensure no data
 * loss.
 */
module.exports = class PersistenceDisk {

	constructor(path, options) {

		this.options = Object.assign({
			/**
			 * Initial value of the data if no data is currently present.
			 */
			initial: {},
			/**
			 * Initial value of the data not persisted.
			 */
			initialNotPersisted: {},
			/**
			 * If initialize should be bypassed, set this flag to false.
			 */
			initialize: true,
			/**
			 * Read the data from a file and returns it.
			 */
			read: async (content) => {
				this.estimatedSize = content.length;
				return JSON.parse(content);
			},
			/**
			 * Write the data to a file.
			 */
			write: (data) => {
				const content = JSON.stringify(data);
				this.estimatedSize = content.length;
				return content;
			},
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
			},
			/**
			 * If set, this will create a task to perform a periodic savepoint.
			 */
			savepointTask: null,
			/**
			 * \brief Perform a savepoint when the close function is called
			 */
			savepointOnClose: true,
			/**
			 * \brief If the delta file exceed a certain size in bytes,
			 *        create a new one.
			 */
			maxDeltaB: 1024 * 1024

		}, options);

		// Initialize local variables
		this.path = path;
		this.mutex = new Mutex();
		this.isSavepoint = false;
		this.event = new Event({
			ready: {proactive: true},
			error: {proactive: true}
		});
		// The estimated size of the structure
		this.estimatedSize = 0;
		// The version is to ensure consitency while creating the savepoint
		this.savepointVersion = 0;
		// Task ID of the periodic savepoint if any
		this.savepointTaskId = null;

		// The persistence needs to be initialized
		this.isInitialized = false;

		// Initialize the persistence
		if (this.options.initialize) {
			this.initialize(/*ignoreErrors*/false);
		}
	}

	/**
	 * Return the estimated size in bytes of the structure
	 */
	get size() {
		return this.estimatedSize;
	}

	/**
	 * \brief Initialize the data by reading the content.
	 */
	async initialize(ignoreErrors) {

		// Lock the mutex to make sure no data is accessed while
		// initializing both the deltas and the datas.
		await this.mutex.lock();

		Exception.assert(!this.savepointTaskId, "A task ID of the periodic task is already created.");

		try {
			// Initialize the delta files
			this.delta = await this.initializeDelta(ignoreErrors);
			// Only if everything went well, set the ready flag
			this.event.trigger("ready");

			// Set the periodic savepoint if needed
			if (this.options.savepointTask !== null) {

				const taskConfig = Object.assign({
					/**
					 * Default namespace for the periodic savepoint, used for the
					 * periodic task.
					 */
					namespace: "persistence-disk",
					/**
					 * Default name for the periodic savepoint, used for the
					 * periodic task.
					 */
					name: this.path,
					/**
					 * Task interval in Ms
					 */
					intervalMs: 0,
					isValid: (iteration) => { return this.isReady() }
				}, this.options.savepointTask);

				this.savepointTaskId = TaskManager.register(taskConfig.namespace, taskConfig.name, () => this.taskSavepoint(), taskConfig);
			}
		}
		catch (e) {
			Exception.fromError(e).print("Error while initializing data");
			this.event.trigger("error", e);
			throw e;
		}
		finally {
			this.mutex.release();
		}
	}

	/**
	 * \brief Initialize the persistence.
	 *
	 * \note This function does not acquire any lock, it assumes
	 *       being already in a secure phase.
	 */
	async initializeDelta(/*ignoreErrors*/) {
		// This is the delta structure that will be initialized and returned
		let delta = {
			list: [],
			dir: this.path + ".delta/",
			dirty: false
		};

		// If there is no directories for delta, create it.
		await this.fileSystemExec("mkdir", delta.dir);

		// List all existing delta files if any
		const fileList = (await this.fileSystemExec("readdir", delta.dir)).filter((file) => file.match(/^[0-9]+\.log$/));

		// Check if dirty, i.e. if a savepoint can be made
		delta.dirty = (fileList.length > 1);
		if (!delta.dirty) {
			for (const i in fileList) {
				if ((await this.fileSystemExec("stat", delta.dir + fileList[i])).size > 0) {
					delta.dirty = true;
					break;
				}
			}
		}

		// List and sort all IDs
		delta.list = fileList.map((name) => {
			return parseInt(name.substring(0, name.indexOf(".")));
		}).sort((a, b) => {
			return a - b;
		});

		return delta;
	}

	/**
	 * \brief Read the data from the persistence
	 */
	async initializeData() {

		// Read the data file if any
		if (await this.fileSystemExec("exists", this.path)) {
			const content = await this.fileSystemExec("readFile", this.path);
			this.data = await this.options.read(content);
		}
		// Initialize the data with the initial ones
		else {
			this.data = Object.assign({}, this.options.initial);
		}

		// Loop through all the deltas and apply them to the data
		for (const i in this.delta.list) {
			// Replay the data synchronously
			await this.applyDelta(this.delta.list[i], this.data);
		}

		// Merge the non-persisted data
		Object.assign(this.data, this.options.initialNotPersisted);

		Log.info("Restored data, replayed " + this.delta.list.length + " log(s)");
	}

	/**
	 * \brief Close the persistence
	 */
	async close() {

		// Unregister the current task
		if (this.options.savepointTask !== null) {
			Exception.assert(this.savepointTaskId, "The task ID for the periodic savepoint");
			TaskManager.unregister(this.savepointTaskId);
			this.savepointTaskId = null;
		}

		// Perform a savepoint if needed
		if (this.options.savepointOnClose) {
			await this.taskSavepoint();
		}

		// Mark as not ready anymore
		this.event.clear("ready");
	}

	/**
	 * \brief Reset the entire persistence to the inital value or a predefined value.
	 */
	async reset(data) {
		// Use the inital value if data is unset
		const setData = (typeof data === "undefined") ? this.options.initial : data;

		// Acquire the lock before performing any operation
		await this.mutex.lock();

		try {
			// Create a temporary file
			const tempPath = this.delta.dir + ".data.reset.temp";
			await this.fileSystemExec("writeFile", tempPath, this.options.write(setData));

			// Replace old data with the new one
			await this.replaceDataNoLock(tempPath);

			// Assign the data and the non persisted data
			this.data = Object.assign({}, setData, this.options.initialNotPersisted);

			Log.debug("Data successfully " + ((typeof data === "undefined") ? "reset" : "set"));

			// Increase the savepoitn version at the end to ensure that if a savepoint
			// is currently running with the old data, it will be discarded.
			this.savepointVersion++;
			// Only if everything went well, set the ready flag
			this.event.trigger("ready");
		}
		catch (e) {
			this.event.trigger("error", e);
			throw e;
		}
		finally {
			this.mutex.release();
		}
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
	 * Wait until all previous operations are completed
	 */
	async waitSync() {
		await this.mutex.lock();
		this.mutex.release();
	}

	/**
	 * \brief Create a new delta file and returns its ID.
	 */
	async createDeltaNoLock() {
		Exception.assert(this.isReady(), "Persistence is not ready yet.");

		// Allocate a new Id
		let newId = (this.delta.list.length) ? (this.delta.list[this.delta.list.length - 1] + 1) : 0;

		// Add it to the list and create an empty file (fails if it already exists)
		{
			this.delta.list.push(newId);
			const path = this.getPathFromId(newId);
			await this.fileSystemExec("close", await this.fileSystemExec("open", path, "wx"));
		}

		Log.debug("Using delta \"" + newId + "\"");

		return newId;
	}

	/**
	 * \brief Register an event
	 */
	on (...args) {
		this.event.on(...args);
	}

	/**
	 * \brief Return the latest version of the data persisted.
	 */
	async get() {
		Exception.assert(this.isReady(), "Persistence is not ready yet");

		// If not in sync, load the persistence
		if (!this.isInitialized) {
			// Load the data
			await this.initializeData();
			this.isInitialized = true;
		}

		return this.data;
	}

	/**
	 * Write new data to the disk
	 *
	 * \param type The type of operation.
	 */
	async write(type, ...args) {
		Exception.assert(this.options.operations.hasOwnProperty(type), () => ("The operation \"" + type + "\" is not supported, valid operations are: " + Object.keys(this.options.operations).join(", ")));

		// This part has to be protected by a mutex in order to make this action atomical
		await this.mutex.lock();

		let data = null;
		try {
			// Ensure the persistence is ready
			Exception.assert(this.isReady(), "Persistence is not ready yet");

			let deltaPath = await this.getDeltaPath();

			// Get the size of the file before appending the data,
			// this is used to recover the file if an error occured.
			const stats = await this.fileSystemExec("stat", deltaPath);
			let fileSize = stats.size;

			// If the size exceed the maxium size, create a new delta file,
			// use it and reset the filesize.
			if (fileSize > this.options.maxDeltaB) {
				await this.createDeltaNoLock();
				deltaPath = await this.getDeltaPath();
				fileSize = 0;
			}

			// Pre-read the current data, it is important to do it before writing to the delta file,
			// otherwise data will be append twice.
			await this.get();

			// Add the action to the delta file, make sure it fits on a single line
			data = JSON.stringify(args).replace(/(?:\r\n|\r|\n)/g, " ");
			await this.fileSystemExec("appendFile", deltaPath, type + " " + data + "\n");

			// Update the current object
			try {
				await this.options.operations[type].call(this, this.data, ...args);

				// If success set the flag to dirty
				this.delta.dirty = true;
			}
			catch (e) {
				// Delete the delta previously added
				await this.fileSystemExec("truncate", deltaPath, fileSize);
				throw Exception.fromError(e);
			}
		}
		finally {
			this.mutex.release();
		}

		Log.debug("Successfully wrote " + type + ": " + data);
	}

	/**
	 * \brief Apply a delta file to the data.
	 */
	async applyDelta(id, data) {

		const path = this.getPathFromId(id);

		Exception.assert(await this.fileSystemExec("exists", path), "File \"" + path + "\" does not exists.");

		// Read the file asynchronously
		const content = await this.fileSystemExec("readFile", path);
		const lineList = content.split("\n").filter((line) => line);

		for (const i in lineList) {

			const line = lineList[i];
			const type = line.substring(0, line.indexOf(" "));
			const argsStr = line.substring(line.indexOf(" ") + 1);

			// Parse the data and apply it
			const args = JSON.parse(argsStr);
			Exception.assert(args instanceof Array, "The data must be an array");
			Exception.assert(this.options.operations.hasOwnProperty(type), () => ("The operation \"" + type + "\" is not supported, valid operations are: " + Object.keys(this.options.operations).join(", ")));

			// Apply the change
			await this.options.operations[type](data, ...args);
		}
	}

	/**
	 * \biref Tells if the perisitence is dirty (hence if it might need a savepoint)
	 */
	isDirty() {
		Exception.assert(this.isReady(), "Persistence is not ready yet");
		return this.delta.dirty;
	}

	/**
	 * Savepoint task, helper to be registered by the task manager
	 */
	async taskSavepoint() {
		if (this.isDirty()) {
			Log.info("Persistence \"" + this.path + "\" is dirty, running savepoint");
			try {
				await this.savepoint();
			}
			catch (e) {
				Exception.fromError(e).print();
			}
		}
	}

	/**
	 * \brief Write the latest version of the data.
	 */
	async savepoint() {
		Exception.assert(this.isReady(), "Persistence is not ready yet");

		// If another savepoint is on-going, cancel
		if (this.isSavepoint) {
			return;
		}
		this.isSavepoint = true;

		// This operation has to be done atomically
		await this.mutex.lock();

		const tempPath = this.delta.dir + ".data.temp";
		let savepointVersion = 0;
		let id = null;
		let promiseWrite = null;

		// Operations to be executed within the lock
		try {
			// Create a new savepoint version
			savepointVersion = ++this.savepointVersion;

			// Create the delta file
			id = await this.createDeltaNoLock();

			// Read the current data
			await this.get();

			// Write the data to a file
			promiseWrite = this.fileSystemExec("writeFile", tempPath, this.options.write(this.data));

			// It will save everything from here. So at this point, if nothing modified
			// the deltas, it will be cleaned.
			this.delta.dirty = false;

			// Release the lock as of now the persistence has already been copied to be written
		}
		catch (e) {
			this.isSavepoint = false;
			Exception.fromError(e).print();
			throw e;
		}
		finally {
			this.mutex.release();
		}

		// This part is done without the lock
		try {
			// Wait until the write is completed
			await promiseWrite;

			// This is the critical part, delete the delta files and replace the current data file this one.
			if (!(await this.savepointReplaceData(tempPath, savepointVersion, id))) {
				Log.warning("The savepoint version differ, meaning that something interfere in parallel");
			}
		}
		catch (e) {
			Exception.fromError(e).print();
			throw e;
		}
		finally {
			this.isSavepoint = false;
		}
	}

	/**
	 * \brief Replace temporary data with the actual ones
	 */
	async savepointReplaceData(tempPath, savepointVersion, lastId) {
		await this.mutex.lock();

		try {

			const isSavepointVersionValid = (savepointVersion === this.savepointVersion);
			// Do the actual savepoint only if the savepoint version matches
			if (isSavepointVersionValid) {
				await this.replaceDataNoLock(tempPath, lastId);
			}

			return isSavepointVersionValid;
		}
		finally {
			this.mutex.release();
		}
	}

	/**
	 * Execute an ansynchronous file system operation with a retry
	 */
	async fileSystemExec(command, ...args) {
		const wait = () => {
			return new Promise((resolve) => {
				setTimeout(() => { resolve(); }, /*ms*/10);
			});
		};

		let retryCounter = 5;

		while (true) {
			try {
				return await FileSystem[command](...args);
			}
			catch (e) {
				if (--retryCounter) {
					await wait();
					continue;
				}
				throw e;
			}
		}
	}

	/**
	 * \brief Replace temporary data with the actual ones
	 *
	 * \note The write lock must be set.
	 */
	replaceDataNoLock(tempPath, lastId) {

		Exception.assert(this.isReady(), "Persistence is not ready yet.");

		return new Promise(async (resolve, reject) => {

			await this.fileSystemExec("move", tempPath, this.path);

			// Delete other files and update the delta list
			while (this.delta.list.length
					// Make sure the id is not the last ID (if defined)
					&& ((typeof lastId === "undefined") || this.delta.list[0] != lastId)) {

				const curId = this.delta.list.shift();
				const path = this.getPathFromId(curId);
				try {
					await this.fileSystemExec("unlink", path);
				}
				catch (e) {
					return reject(new Exception(e));
				}
			}

			// If the list is empty, mark the dirty flag as false
			if (!this.delta.list.length) {
				this.delta.dirty = false;
			}

			resolve();
		});
	}

	/**
	 * \brief Return the current delta path.
	 *
	 * \note This function must be under a lock.
	 */
	async getDeltaPath() {
		// If there is no file currently, add one
		if (!this.delta.list.length) {
			await this.createDeltaNoLock();
		}
		const id = this.delta.list[this.delta.list.length - 1];
		const path = this.getPathFromId(id);

		return path;
	}

	/**
	 * \brief Generate the path of a delta file from its ID.
	 */
	getPathFromId(id) {
		return this.delta.dir + id + ".log";
	}

	/**
	 * This function is for testing only. It ensures that
	 * the content is consistent.
	 */
	async consistencyCheck() {
		Exception.assert(this.isReady(), "Persistence is not ready yet.");

		// Make sure that the content in the list is the same
		// as the content on the directory
		await this.mutex.lock();

		try {
			const delta = await this.initializeDelta(/*ignoreErrors*/false);

			Exception.assert(delta.list.length == this.delta.list.length, () => ("Deltas on disk " + JSON.stringify(delta.list)
					+ " differs from deltas in memory" + JSON.stringify(this.delta.list)));
			Exception.assert(delta.list.every((id, index) => (this.delta.list[index] == id)), () => ("Deltas on disk " + JSON.stringify(delta.list)
					+ " differs from deltas in memory" + JSON.stringify(this.delta.list)));

			{
				let prevId = -1;
				Exception.assert(delta.list.every((id) => {
					const isIncreasing = (id > prevId);
					prevId = id;
					return isIncreasing;
				}), "Delta list is inconsistent");
			}

			// Check the dirty flag
			Exception.assert(this.delta.dirty || (delta.dirty == this.delta.dirty), "Dirty flag is wrong: " + JSON.stringify(delta) + ", " + JSON.stringify(this.delta));
		}
		finally {
			this.mutex.release();
		}

		return true;
	}
};
