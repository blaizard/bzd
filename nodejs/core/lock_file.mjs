import fs from "fs/promises";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import ExceptionFactory from "./exception.mjs";

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

	constructor(path) {
		this.path = pathlib.path(path);
		this.interval = null;
		this.lastExpiryCheck = null;
		this.heartBeatIntervalMs = 5000;
	}

	/// Return the current timestamp in seconds.
	_timestampS() {
		return Date.now() / 1000;
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
			await fs.mkdir(path);
			return true;
		} catch (error) {
			Exception.assert(error.code == "EEXIST", "Error attempting to create lock directory: {}", error);
		}
		return false;
	}

	async _unlock(path) {
		await fs.rm(path, { recursive: true, force: true });
	}

	/// Check if the status of the lock.
	///
	/// \return true if the lock is expired, false otherwise.
	async getStatus() {
		const path = this.path.asPosix();
		const currentTimeS = this._timestampS();
		try {
			const stats = await fs.stat(path);
			const fileTimeMs = stats.mtime.getTime();

			// Update the last expiry check time.
			if (this.lastExpiryCheck === null) {
				this.lastExpiryCheck = [fileTimeMs, currentTimeS];
			} else if (this.lastExpiryCheck[0] == fileTimeMs) {
				// Do nothing, the file has not been updated since last check.
			} else {
				this.lastExpiryCheck = [fileTimeMs, currentTimeS];
			}
		} catch (error) {
			if (error.code === "ENOENT") {
				return LockFile.Status.unlocked;
			}
			throw error;
		}

		// Check if the lock is expired (no update in the last 15s).
		if (currentTimeS - this.lastExpiryCheck[1] > this.heartBeatIntervalMs * 3) {
			return LockFile.Status.expired;
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
				const timestampS = this._timestampS();
				try {
					await fs.utimes(path, timestampS, timestampS);
				} catch (e) {
					clearInterval(this.interval);
					this.interval = null;
					Exception.unreachable("Lock heartbeat failed: {}", e);
				}
			};
			await heartBeat();
			this.interval = setInterval(heartBeat, this.heartBeatIntervalMs).unref();
			return true;
		}

		return false;
	}

	/// Unlock the pre-acquired lock.
	async unlock() {
		Exception.assert(this.isLock(), "Lock is not acquired.");
		await this._unlock(this.path.asPosix());
		clearInterval(this.interval);
		this.interval = null;
	}

	/// Execute the given function with the lock acquired and release it afterwards.
	async lock(workFn, timeoutS = null) {
		Exception.assert(!this.isLock(), "Lock is already acquired.");
		const startTime = this._timestampS();
		while (!(await this.tryLock())) {
			await new Promise((resolve) => setTimeout(resolve, 1));
			Exception.assert(
				!timeoutS || this._timestampS() - startTime < timeoutS,
				"Timeout acquiring lock after {}s.",
				timeoutS,
			);
		}
		try {
			return await workFn();
		} finally {
			await this.unlock();
		}
	}
}
