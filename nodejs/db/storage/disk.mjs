

import Path from "path";
import Fs from "fs";

import Storage from "./storage.mjs";
import FileSystem from "../../core/filesystem.mjs";

/**
 * File storage module
 */
export default class StorageDisk extends Storage {

	constructor(path, options) {
		super();

		this.options = Object.assign({
		}, options);
		this.path = path;

		this._initialize();
	}

	/**
	 * Initialize the storage module
	 */
	async _initializeImpl() {
		await FileSystem.mkdir(this.path);
	}

	_getPath(bucket, key = undefined) {
		return (key) ? Path.join(this.path, bucket, key) : Path.join(this.path, bucket);
	}

	async is(bucket, key) {
		return await FileSystem.exists(this._getPath(bucket, key));
	}

	async read(bucket, key) {
		return Fs.createReadStream(this._getPath(bucket, key));
	}

	async write(bucket, key, data) {
		const path = this._getPath(bucket, key);
		await FileSystem.mkdir(Path.dirname(path));
		let writeStream = Fs.createWriteStream(path);

		let readStream = data;
		if (typeof data == "string") {
			readStream = Fs.createReadStream(data);
		}

		return new Promise((resolve, reject) => {
			readStream
				.on("error", reject)
				.on("end", resolve)
				.on("finish", resolve)
				.pipe(writeStream);
		});
	}

	async delete(bucket, key) {
		await FileSystem.unlink(this._getPath(bucket, key));
	}

	async list(bucket) {
		const path = this._getPath(bucket);
		if (await FileSystem.exists(path)) {
			return await FileSystem.readdir(path);
		}
		return [];
	}
}
