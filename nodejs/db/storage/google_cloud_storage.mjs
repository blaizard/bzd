import CloudAPI from "@google-cloud/storage";

import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import { copy as copyStream } from "../../core/stream.mjs";
import { CollectionPaging } from "../utils.mjs";

import Permissions from "./permissions.mjs";
import Base from "./storage.mjs";

const Storage = CloudAPI.Storage;
const Log = LogFactory("db", "storage", "google-cloud-storage");
const Exception = ExceptionFactory("db", "storage", "google-cloud-storage");

/**
 * File storage module
 */
export default class StorageGoogleCloudStorage extends Base {
	constructor(bucketName, options) {
		super();

		this.options = Object.assign(
			{
				prefix: null,
			},
			options,
		);
		this.storage = new Storage();
		this.bucketName = bucketName;
		this.bucket = this.storage.bucket(this.bucketName);

		Log.info("Using Google cloud storage DB with bucket '{}' and prefix '{}'.", bucketName, this.options.prefix);
	}

	async _initialize() {
		const result = await this.bucket.exists();
		if (!result[0]) {
			await this.storage.createBucket(this.bucketName);
		}
	}

	_getFullPath(pathList) {
		return this.options.prefix ? this.options.prefix + "/" + pathList.join("/") : pathList.join("/");
	}

	_getFile(pathList) {
		return this.bucket.file(this._getFullPath(pathList));
	}

	async _isImpl(pathList) {
		const file = this._getFile(pathList);
		const result = await file.exists();
		return result[0];
	}

	async _readImpl(pathList) {
		return this._getFile(pathList).createReadStream();
	}

	async _delay(timeMs) {
		return new Promise((resolve) => {
			setTimeout(resolve, timeMs);
		});
	}

	async _waitUntilExists(pathList, timeoutMs = 10000) {
		let timeMs = 0;
		while (!(await this._isImpl(pathList)) && timeMs < timeoutMs) {
			await this._delay(1000);
			timeMs += 1000;
		}
		Exception.assert(await this._isImpl(pathList), "File path='{:j}' does not exists", pathList);
	}

	async _writeImpl(pathList, readStream) {
		const file = this._getFile(pathList);
		let writeStream = file.createWriteStream();
		await copyStream(writeStream, readStream);
		await this._waitUntilExists(pathList);
	}

	async _deleteImpl(pathList) {
		const file = this._getFile(pathList);
		await file.delete();
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const prefix = this._getFullPath(pathList);
		const [files, apiResponse] = await this.bucket.getFiles({
			autoPaginate: false,
			prefix: prefix,
			maxResults: paging.max,
			pageToken: paging.page || undefined,
		});
		let result = [];
		for (const file of files) {
			const name = file.metadata.name.slice(prefix.length + 1);
			result.push(
				includeMetadata
					? Permissions.makeEntry(
							{
								name: name,
								size: parseInt(file.metadata.size),
								type: file.metadata.contentType,
								modified: file.metadata.updated,
								created: file.metadata.timeCreated,
							},
							{
								read: true,
								write: true,
								delete: true,
							},
						)
					: name,
			);
		}
		if (apiResponse && "pageToken" in apiResponse) {
			return new CollectionPaging(result, { page: apiResponse.pageToken, max: paging.max });
		}
		return new CollectionPaging(result);
	}
}
