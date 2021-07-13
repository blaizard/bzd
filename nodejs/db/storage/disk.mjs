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
/**
 * File storage module
 */
export default class StorageDisk extends Storage {
	constructor(path, options) {
		super();

		this.options = Object.assign(
			{
				/**
				 * If false, it will attempt to create a directory if it does not exists.
				 */
				mustExists: false
			},
			options
		);
		this.path = path;

		Log.info("Using disk storage DB at '{}'.", this.path);
	}

	/**
	 * Initialize the storage module
	 */
	async _initialize() {
		if (!(await FileSystem.exists(this.path))) {
			Exception.assert(!this.options.mustExists, "'{}' is not a valid path.", this.path);
			await FileSystem.mkdir(this.path);
		}
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
					return Permissions.makeEntry(
						{
							name: dirent.name,
							type: dirent.isDirectory() ? "directory" : Path.extname(dirent.name).slice(1),
							size: stat.size,
							created: stat.ctime,
							modified: stat.mtime
						},
						{
							read: dirent.isDirectory() ? false : true,
							write: dirent.isDirectory() ? false : true,
							delete: dirent.isDirectory() ? false : true,
							list: dirent.isDirectory() ? true : false
						}
					);
				});
			}
			return await CollectionPaging.makeFromList(data, maxOrPaging);
		}
		return new CollectionPaging([]);
	}
}
