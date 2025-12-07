import Crypto from "crypto";

import LockFile from "#bzd/nodejs/core/lock_file.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";
import ServicesProvider from "#bzd/nodejs/core/services/provider.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";

const Exception = ExceptionFactory("locks");
const Log = LogFactory("locks");

/// Cross process lock pool.
export default class Locks {
	constructor(path, options = {}) {
		// Root directory where all the locks are stored.
		Exception.assert(Boolean(path), "Path cannot be null or empty.");
		this.path = pathlib.path(path);
		this.options = Object.assign(
			{
				statistics: new StatisticsProvider(),
				services: new ServicesProvider(),
				fs: FileSystem,
			},
			options,
		);

		this.options.fs.mkdirSync(this.path.asPosix());
		// Keep track of open locks for the garbage collection.
		this.locks = {};

		// Register the garbage collector.
		this.options.services.addTimeTriggeredProcess(
			"garbage.collector",
			async () => {
				// Update the locks map.
				const files = await this.options.fs.readdir(this.path.asPosix(), /*withFileTypes*/ true);
				this.locks = Object.fromEntries(
					files
						.filter((dirent) => dirent.isDirectory())
						.map((dirent) => {
							const name = dirent.name;
							return [
								name,
								name in this.locks
									? this.locks[name]
									: new LockFile(this.path.joinPath(name).asPosix(), { fs: this.options.fs }),
							];
						}),
				);

				for (const [name, lock] of Object.entries(this.locks)) {
					const status = await lock.getStatus();
					if (status == LockFile.Status.expired) {
						await lock.unlock(/*force*/ true);
						Exception.unreachable("Dangling lock with status expired: '{}', removed.", name);
					}
				}
			},
			{
				periodS: 60,
			},
		);
	}

	/// Get a unique hash for the given path.
	_getInternalPath(path) {
		return this.path.joinPath(Crypto.createHash("sha1").update(path).digest("hex"));
	}

	/// Try to acquire a lock for the given path.
	///
	/// \param path The path to lock.
	/// \return the LockFile instance if the lock was acquired, null otherwise.
	async tryLock(path) {
		const internalPath = this._getInternalPath(path);
		const lock = new LockFile(internalPath, {
			fs: this.options.fs,
		});
		if (await lock.tryLock()) {
			return lock;
		}
		return null;
	}

	/// Acquire a lock for the given path and execute the workFn.
	///
	/// \param path The path to lock.
	/// \param workFn The function to execute while holding the lock.
	/// \param timeoutS Optional timeout in seconds for the lock.
	/// \return The result of the workFn.
	async lock(path, workFn, timeoutS = null) {
		const internalPath = this._getInternalPath(path);
		const lock = new LockFile(internalPath, {
			fs: this.options.fs,
		});
		return await lock.lock(workFn, timeoutS);
	}
}
