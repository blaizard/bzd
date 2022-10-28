import Path from "path";
import Fs from "fs";

import Storage from "./storage.mjs";
import FileSystem from "../../core/filesystem.mjs";
import LogFactory from "../../core/log.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import { copy as copyStream } from "../../core/stream.mjs";
import { CollectionPaging } from "../utils.mjs";
import Permissions from "./permissions.mjs";

const Log = LogFactory("db", "storage", "disk");
const Exception = ExceptionFactory("db", "storage", "disk");

/// File storage module
export default class StorageDisk extends Storage {
	constructor(path, options) {
		super();

		this.options = Object.assign(
			{
				/// If false, it will attempt to create a directory if it does not exists.
				mustExists: false,
			},
			options
		);
		this.path = path;

		Log.info("Using disk storage DB at '{}'.", this.path);
	}

	/// Initialize the storage module
	async _initialize() {
		if (!(await FileSystem.exists(this.path))) {
			Exception.assert(!this.options.mustExists, "'{}' is not a valid path.", this.path);
			await FileSystem.mkdir(this.path);
		}
	}

	_getFullPath(pathList) {
		return Path.join(this.path, ...pathList);
	}

	async _isImpl(pathList) {
		return await FileSystem.exists(this._getFullPath(pathList));
	}

	async _readImpl(pathList) {
		return Fs.createReadStream(this._getFullPath(pathList));
	}

	async _writeImpl(pathList, readStream) {
		const fullPath = this._getFullPath(pathList);
		await FileSystem.mkdir(Path.dirname(fullPath));
		let writeStream = Fs.createWriteStream(fullPath);

		return copyStream(writeStream, readStream);
	}

	async _deleteImpl(pathList) {
		await FileSystem.unlink(this._getFullPath(pathList));
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const fullPath = this._getFullPath(pathList);
		if (await FileSystem.exists(fullPath)) {
			const data = await FileSystem.readdir(fullPath, /*withFileTypes*/ includeMetadata);
			if (includeMetadata) {
				return await CollectionPaging.makeFromList(data, maxOrPaging, async (dirent) => {
					let filePath = fullPath + "/" + dirent.name;
					let isRead = true;
					let isWrite = true;
					let isDelete = true;
					let isList = true;
					let type = "";

					if (dirent.isSymbolicLink()) {
						// Try to resolve the actual path.
						try {
							filePath = await FileSystem.realpath(filePath);
						}
						catch (e) {
							isRead = false;
							isWrite = false;
							isDelete = false;
							isList = false;
						}
					}
					const stat = await FileSystem.stat(filePath);

					if (stat.isFile()) {
						isList = false;
						type = Path.extname(dirent.name).slice(1);
					}
					else if (stat.isDirectory()) {
						isRead = false;
						isWrite = false;
						isDelete = false;
						type = "directory";
					}
					else {
						isRead = false;
						isWrite = false;
						isDelete = false;
						isList = false;
						if (stat.isBlockDevice() || stat.isCharacterDevice()) {
							type = "device";
						}
						else if (stat.isFIFO()) {
							type = "pipe";
						}
						else if (stat.isSocket()) {
							type = "socket";
						}
					}

					return Permissions.makeEntry(
						{
							name: dirent.name,
							type: type,
							size: stat.size,
							created: stat.ctime,
							modified: stat.mtime,
						},
						{
							read: isRead,
							write: isWrite,
							delete: isDelete,
							list: isList,
						}
					);
				});
			}
			return await CollectionPaging.makeFromList(data, maxOrPaging);
		}
		return new CollectionPaging([]);
	}
}
