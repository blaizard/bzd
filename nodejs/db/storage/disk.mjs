"use strict";

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

	async is(bucket, key) {
		return await FileSystem.exists(Path.join(this.path, bucket, key));
	}

	async read(bucket, key) {
		return Fs.createReadStream(Path.join(this.path, bucket, key));
	}

	async write(bucket, key, readStream) {
		let writeStream = Fs.createWriteStream(Path.join(this.path, bucket, key));

		return new Promise((resolve, reject) => {
			readStream.pipe(writeStream)
				.on("error", reject)
				.on("end", resolve);
		});
	}
}
