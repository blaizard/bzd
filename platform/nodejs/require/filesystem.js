"use strict";

const Path = require("path");
const Fs = require("fs");

/**
 * Collection of Promise-based asynchronous functions for the file system.
 */
class FileSystem {

	/**
	 * Checks if the file or directory exists
	 */
	static async exists(path) {
		return new Promise((resolve) => {
			Fs.access(path, Fs.F_OK, (e) => {
				resolve((e) ? false : true);
			});
		});
	}

	/**
	 * Get the stat object associated with a file
	 */
	static async stat(path) {
		return new Promise((resolve, reject) => {
			Fs.stat(path, (e, stats) => {
				return (e) ? reject(e) : resolve(stats);
			});
		});
	}

	/**
	 * Create a directory recursively, if it already exists, do nothing
	 */
	static async mkdir(path) {
		let curPath = Path.resolve(path);
		let dirList = [];

		// Identify the directories that do not exists
		while (!(await FileSystem.exists(curPath))) {
			dirList.unshift(Path.basename(curPath));
			curPath = Path.dirname(curPath);
		}

		// Create them
		while (dirList.length) {
			curPath = Path.join(curPath, dirList.shift());
			try {
				await mkdir(curPath);
			}
			catch (e) {
				// If the directory has been created in parallel, ignore this error
				if (e.code == "EEXIST") {
					continue;
				}
				throw e;
			}
		}
	}

	/**
	 * Remove a directory recursively
	 */
	static async rmdir(path) {

		return new Promise(async (resolve, reject) => {

			try {
				const fileList = await FileSystem.readdir(path);
				for (const i in fileList) {
					const curPath = Path.resolve(path, fileList[i]);
					const stat = await FileSystem.stat(curPath);
					if (stat.isDirectory()) {
						await FileSystem.rmdir(curPath);
					}
					else {
						await FileSystem.unlink(curPath);
					}
				}
			}
			catch (e) {
				return reject(e);
			}

			Fs.rmdir(path, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Read the content of a file
	 */
	static async readFile(path, options = "utf8") {
		return new Promise((resolve, reject) => {
			Fs.readFile(path, options, (e, data) => {
				return (e) ? reject(e) : resolve(data.toString());
			});
		});
	}

	/**
	 * Write the content of a file
	 */
	static async writeFile(path, data, options = "utf8") {
		return new Promise((resolve, reject) => {
			Fs.writeFile(path, data, options, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Read the content of a directory
	 */
	static async readdir(path) {
		return new Promise((resolve, reject) => {
			Fs.readdir(path, (e, dataList) => {
				return (e) ? reject(e) : resolve(dataList);
			});
		});
	}

	/**
	 * Move a file from a location to another. Note pathFrom and pathTo
	 * are the full path of the file including the file name.
	 */
	static async move(pathFrom, pathTo) {
		return new Promise((resolve, reject) => {
			Fs.rename(pathFrom, pathTo, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Copy a file from a location to another. Note pathFrom and pathTo
	 * are the full path of the file including the file name.
	 * If the destination file already exists, it will fail.
	 */
	static async copy(pathFrom, pathTo) {
		return new Promise((resolve, reject) => {
			Fs.copyFile(pathFrom, pathTo, Fs.constants.COPYFILE_EXCL, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Delete a file
	 */
	static async unlink(path) {
		return new Promise((resolve, reject) => {
			Fs.unlink(path, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Append data to a file
	 */
	static async appendFile(path, data) {
		return new Promise((resolve, reject) => {
			Fs.appendFile(path, data, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Truncate a file to a specific size
	 */
	static async truncate(path, fileSize) {
		return new Promise((resolve, reject) => {
			Fs.truncate(path, fileSize, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}

	/**
	 * Touch a file (create it if it does not exists) and
	 * updates its last modification date.
	 */
	static async touch(path) {
		return new Promise(async (resolve, reject) => {
			try {
				const fd = await FileSystem.open(path, "a");
				await FileSystem.close(fd);
				resolve();
			}
			catch (e) {
				reject(e);
			}
		});
	}

	/**
	 * Open a file asynchronously
	 */
	static async open(path, options) {
		return new Promise((resolve, reject) => {
			Fs.open(path, options, (e, fd) => {
				return (e) ? reject(e) : resolve(fd);
			});
		});
	}

	/**
	 * Close a file asynchronously, previously open with "open"
	 */
	static async close(fd) {
		return new Promise((resolve, reject) => {
			Fs.close(fd, (e) => {
				return (e) ? reject(e) : resolve();
			});
		});
	}
};

// ---- Helper ----------------------------------------------------------------

function mkdir(path)
{
	new Promise((resolve, reject) => {
		Fs.mkdir(path, (e) => {
			(e) ? reject(e) : resolve();
		});
	});
}

module.exports = FileSystem;
