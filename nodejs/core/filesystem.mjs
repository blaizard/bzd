import Path from "path";
import Fs from "fs";

/// Collection of Promise-based asynchronous functions for the file system.
export default class FileSystem {
	/// Checks if the file or directory exists
	static async exists(path) {
		return new Promise((resolve) => {
			Fs.access(path, Fs.F_OK, (e) => {
				resolve(e ? false : true);
			});
		});
	}

	/// Get the stat object associated with a file
	static async stat(path) {
		return new Promise((resolve, reject) => {
			Fs.stat(path, (e, stats) => {
				return e ? reject(e) : resolve(stats);
			});
		});
	}

	/// Create a directory recursively, if it already exists, do nothing
	static async mkdir(path) {
		await Fs.promises.mkdir(path, {
			recursive: true,
		});
	}

	/// Remove a directory recursively
	static async rmdir(path) {
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

		await Fs.promises.rmdir(path);
	}

	/// Read the content of a file
	static async readFile(path, options = "utf8") {
		const data = await Fs.promises.readFile(path, {
			encoding: options,
		});
		return data.toString();
	}

	/// Write the content of a file
	static async writeFile(path, data, options = "utf8") {
		await Fs.promises.writeFile(path, data, {
			encoding: options,
		});
	}

	/// Read the content of a directory
	static async readdir(path, withFileTypes = false) {
		return await Fs.promises.readdir(path, { withFileTypes: withFileTypes });
	}

	/// Move a file from a location to another. Note pathFrom and pathTo
	/// are the full path of the file including the file name.
	static async move(pathFrom, pathTo) {
		await Fs.promises.rename(pathFrom, pathTo);
	}

	/// Rename a file.
	static async rename(pathFrom, name) {
		const pathTo = Path.join(Path.dirname(pathFrom), name);
		await Fs.promises.rename(pathFrom, pathTo);
	}

	/// Copy a file from a location to another. Note pathFrom and pathTo
	/// are the full path of the file including the file name.
	/// If the destination file already exists, it will fail.
	static async copy(pathFrom, pathTo) {
		await Fs.promises.copyFile(pathFrom, pathTo, Fs.constants.COPYFILE_EXCL);
	}

	/// Delete a file
	static async unlink(path) {
		await Fs.promises.unlink(path);
	}

	/// Changes the permissions of a file.
	static async chmod(path, mode) {
		await Fs.promises.chmod(path, mode);
	}

	/// Append data to a file
	static async appendFile(path, data) {
		await Fs.promises.appendFile(path, data);
	}

	/// Truncate a file to a specific size
	static async truncate(path, fileSize) {
		await Fs.promises.truncate(path, fileSize);
	}

	/// Touch a file (create it if it does not exists) and
	/// updates its last modification date.
	static async touch(path) {
		const fd = await FileSystem.open(path, "a");
		await FileSystem.close(fd);
	}

	/// Open a file asynchronously
	static async open(path, options) {
		return await Fs.promises.open(path, options);
	}

	/// Close a file asynchronously, previously open with "open"
	static async close(fileHandle) {
		await fileHandle.close();
	}

	/// Determines the actual location of a path and resolve symlinks.
	static async realpath(path, options) {
		return await Fs.promises.realpath(path, options);
	}

	/// Creates a symbolic link.
	static async symlink(target, path) {
		return await Fs.promises.symlink(target, path);
	}

	/// Creates a hard link.
	static async hardlink(target, path) {
		return await Fs.promises.link(target, path);
	}

	/// Walk through a directory and print all files
	static async *walk(path) {
		const dirents = await Fs.promises.readdir(path, { withFileTypes: true });
		for (const dirent of dirents) {
			const res = Path.resolve(path, dirent.name);
			if (dirent.isDirectory()) {
				yield* FileSystem.walk(res);
			}
			else {
				yield res;
			}
		}
	}
}
