import Crypto from "crypto";

import LockFile from "#bzd/nodejs/core/lock_file.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";

const Exception = ExceptionFactory("plugin");

/// Cross process lock pool.
export default class Locks {
	constructor(path, options = {}) {
		// Root directory where all the locks are stored.
		Exception.assert(Boolean(path), "Path cannot be null or empty.");
		this.path = pathlib.path(path);

		this.options = Object.assign(
			{
				statistics: new StatisticsProvider(),
				fs: FileSystem,
			},
			options,
		);
	}

	/// Get a unique hash for the given path.
	_getInternalPath(path) {
		return this.path.join(Crypto.createHash("sha1").update(path).digest("hex"));
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
