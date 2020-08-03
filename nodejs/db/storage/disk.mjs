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

	_getFullPath(bucket, key = undefined) {
		return key ? Path.join(this.path, bucket, key) : Path.join(this.path, bucket);
	}

	async _isImpl(path, name) {
		return await FileSystem.exists(this._getFullPath(path, name));
	}

	async _readImpl(path, name) {
		return Fs.createReadStream(this._getFullPath(path, name));
	}

	async _writeImpl(path, name, readStream) {
		const fullPath = this._getFullPath(path, name);
		await FileSystem.mkdir(Path.dirname(fullPath));
		let writeStream = Fs.createWriteStream(fullPath);

		return copyStream(writeStream, readStream);
	}

	async _deleteImpl(path, name) {
		await FileSystem.unlink(this._getFullPath(path, name));
	}

	async _listImpl(path, maxOrPaging, includeMetadata) {
		const fullPath = this._getFullPath(path);
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
						modified: stat.mtime,
					};
				});
			}
			return await CollectionPaging.makeFromList(data, maxOrPaging);
		}
		return new CollectionPaging([]);
	}
}
