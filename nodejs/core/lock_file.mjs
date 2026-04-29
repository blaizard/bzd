import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import ExceptionFactory from "./exception.mjs";
import ClockDate from "#bzd/nodejs/core/clock/date.mjs";
import FileSystem from "./filesystem.mjs";

const Exception = ExceptionFactory("lock-file");

/// Simple file based lock.
///
/// This is lock is meant to work across processes.
export default class LockFile {
	static Status = Object.freeze({
		// It is unlocked.
		unlocked: "unlocked",
		// It is locked.
		locked: "locked",
		// It is locked but the lock is expired.
		expired: "expired",
	});

	constructor(path, options = {}) {
		this.path = pathlib.path(path);
		this.options = Object.assign(
			{
				// Clock instance to use for time retrieval.
				clock: new ClockDate(),
				// file system layer.
				fs: FileSystem,
				// Heartbeat interval in milliseconds.
				heartBeatIntervalMs: 5000,
			},
			options,
		);
		this.interval = null;
		this.intervalActive = false;
		this.lastExpiryCheck = null;
	}

	/// Whether or not the lock is already acquired by this instance.
	isLock() {
		return this.interval !== null;
	}

	/// Try to lock a file by creating it atomically.
	///
	/// \return true of false, whether the lock was acquired of not.
	async _tryLock(path) {
		try {
			await this.options.fs.mkdir(path, { force: false });
			return true;
		} catch (error) {
			Exception.assert(error.code == "EEXIST", "Error attempting to create lock directory: {}", error);
		}
		return false;
	}

	async _unlock(path, force) {
		this.lastExpiryCheck = null;
		// force = true, will silently ignore if the path does not exist.
		await this.options.fs.rmdir(path, { force: force });
	}

	/// Wait for a certain amount of time in milliseconds.
	async _wait(timeMs) {
		return new Promise((resolve) => setTimeout(resolve, timeMs));
	}

	/// Stop the interval function and wait until it is completed.
	async _stopInterval(timeoutS) {
		clearInterval(this.interval);
		this.interval = null;

		// Wait for the interval to stop, the code should very rarely go there.
		const startTime = this.options.clock.getTimeS();
		while (this.intervalActive) {
			await this._wait(1);
			Exception.assert(
				!timeoutS || this.options.clock.getTimeS() - startTime < timeoutS,
				"Timeout releasing lock after {}s.",
				timeoutS,
			);
		}
	}

	/// Check if the status of the lock.
	///
	/// \return the status of the lock.
	async getStatus() {
		const path = this.path.asPosix();
		const currentTimeMs = this.options.clock.getTimeMs();
		try {
			const stats = await this.options.fs.stat(path);
			const fileTimeMs = stats.mtime.getTime();

			// Update the last expiry check time.
			const thresholdMs = Math.max(this.options.heartBeatIntervalMs * 3, 2000);
			if (this.lastExpiryCheck === null) {
				// If the lock is already older than the threshold, initialize it as expired.
				if (currentTimeMs - fileTimeMs > thresholdMs) {
					this.lastExpiryCheck = [fileTimeMs, currentTimeMs - thresholdMs - 1];
				} else {
					this.lastExpiryCheck = [fileTimeMs, currentTimeMs];
				}
			} else if (this.lastExpiryCheck[0] == fileTimeMs) {
				// Do nothing, the file has not been updated since last check.
			} else {
				this.lastExpiryCheck = [fileTimeMs, currentTimeMs];
			}

			// Check if the lock is expired.
			if (currentTimeMs - this.lastExpiryCheck[1] > thresholdMs) {
				return LockFile.Status.expired;
			}
		} catch (error) {
			if (error.code === "ENOENT") {
				return LockFile.Status.unlocked;
			}
			throw error;
		}

		return LockFile.Status.locked;
	}

	/// Try to acquire the lock.
	///
	/// \return true if the lock was acquired, false otherwise.
	async tryLock() {
		Exception.assert(!this.isLock(), "Lock is already acquired.");
		const path = this.path.asPosix();

		if (await this._tryLock(path)) {
			// Update the mtime of the file using file system time.
			const heartBeat = async () => {
				if (this.intervalActive) {
					return;
				}
				const timestampS = this.options.clock.getTimeS();
				try {
					this.intervalActive = true;
					await this.options.fs.utimes(path, timestampS, timestampS);
					return true;
				} catch (e) {
					if (this.interval) {
						clearInterval(this.interval);
						this.interval = null;
						this.lastExpiryCheck = null;
					}
					return false;
				} finally {
					this.intervalActive = false;
				}
			};
			this.lastExpiryCheck = null;
			if (!(await heartBeat())) {
				return false;
			}
			this.interval = setInterval(heartBeat, this.options.heartBeatIntervalMs).unref();
			return true;
		}

		return false;
	}

	/// Unlock the pre-acquired lock.
	///
	/// \param force Whether or not to force unlock even if not locked. This is useful for cleanup.
	async unlock(force = false, timeoutS = 5) {
		Exception.assert(force || this.isLock(), "Lock is not acquired.");

		try {
			await this._stopInterval(timeoutS);
		} finally {
			await this._unlock(this.path.asPosix(), force);
		}
	}

	/// Execute the given function with the lock acquired and release it afterwards.
	async lock(workFn, timeoutS = null) {
		Exception.assert(!this.isLock(), "Lock is already acquired.");
		const startTime = this.options.clock.getTimeS();
		while (!(await this.tryLock())) {
			await this._wait(10);
			Exception.assert(
				!timeoutS || this.options.clock.getTimeS() - startTime < timeoutS,
				"Timeout acquiring lock after {}s.",
				timeoutS,
			);
		}
		try {
			return await workFn();
		} finally {
			await this.unlock(/*force*/ true);
		}
	}
}
