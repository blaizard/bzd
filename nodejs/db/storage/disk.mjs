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

	_getPath(bucket, key = undefined) {
		return key ? Path.join(this.path, bucket, key) : Path.join(this.path, bucket);
	}

	async _isImpl(bucket, key) {
		return await FileSystem.exists(this._getPath(bucket, key));
	}

	async _readImpl(bucket, key) {
		return Fs.createReadStream(this._getPath(bucket, key));
	}

	async _writeImpl(bucket, key, readStream) {
		const path = this._getPath(bucket, key);
		await FileSystem.mkdir(Path.dirname(path));
		let writeStream = Fs.createWriteStream(path);

		return copyStream(writeStream, readStream);
	}

	async _deleteImpl(bucket, key) {
		await FileSystem.unlink(this._getPath(bucket, key));
	}

	async _listImpl(bucket, maxOrPaging/*, includeMetadata*/) {
		const path = this._getPath(bucket);
		if (await FileSystem.exists(path)) {
			const data = await FileSystem.readdir(path);
			return CollectionPaging.makeFromList(data, maxOrPaging);
		}
		return new CollectionPaging([]);
	}
}
