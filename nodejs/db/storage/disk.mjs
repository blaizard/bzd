import Path from "path";
import Fs from "fs";

import Storage from "./storage.mjs";
import FileSystem from "../../core/filesystem.mjs";
import LogFactory from "../../core/log.mjs";
import { copy as copyStream } from "../../core/stream.mjs";
import { CollectionPaging } from "../utils.mjs";

const Log = LogFactory("db", "storage", "disk");
/**
 * File storage module
 */
export default class StorageDisk extends Storage {
	constructor(path, options) {
		super();

		this.options = Object.assign({}, options);
		this.path = path;

		Log.info("Using disk storage DB at '{}'.", this.path);
		this._initialize();
	}

	/**
	 * Initialize the storage module
	 */
	async _initializeImpl() {
		await FileSystem.mkdir(this.path);
	}

	_getFullPath(pathList, name = undefined) {
		return name ? Path.join(this.path, ...pathList, name) : Path.join(this.path, ...pathList);
	}

	async _isImpl(pathList, name) {
		return await FileSystem.exists(this._getFullPath(pathList, name));
	}

	async _readImpl(pathList, name) {
		return Fs.createReadStream(this._getFullPath(pathList, name));
	}

	async _writeImpl(pathList, name, readStream) {
		const fullPath = this._getFullPath(pathList, name);
		await FileSystem.mkdir(Path.dirname(fullPath));
		let writeStream = Fs.createWriteStream(fullPath);

		return copyStream(writeStream, readStream);
	}

	async _deleteImpl(pathList, name) {
		await FileSystem.unlink(this._getFullPath(pathList, name));
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const fullPath = this._getFullPath(pathList);
		if (await FileSystem.exists(fullPath)) {
			const data = await FileSystem.readdir(fullPath, /*withFileTypes*/ includeMetadata);
			if (includeMetadata) {
				return await CollectionPaging.makeFromList(data, maxOrPaging, async (dirent) => {
					const stat = await FileSystem.stat(fullPath + "/" + dirent.name);
					return {
						name: dirent.name,
						type: dirent.isDirectory() ? "directory" : Path.extname(dirent.name).slice(1),
						size: stat.size,
						created: stat.ctime,
						modified: stat.mtime
					};
				});
			}
			return await CollectionPaging.makeFromList(data, maxOrPaging);
		}
		return new CollectionPaging([]);
	}
}
