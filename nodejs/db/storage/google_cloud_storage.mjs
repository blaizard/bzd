import Base from "./storage.mjs";
import LogFactory from "../../core/log.mjs";
import CloudAPI from "@google-cloud/storage";
import { copy as copyStream } from "../../core/stream.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import { CollectionPaging } from "../utils.mjs";

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
				prefix: "default",
			},
			options
		);
		this.storage = new Storage();
		this.bucketName = bucketName;
		this.bucket = this.storage.bucket(this.bucketName);

		Log.info("Using Google cloud storage DB with bucket '{}' and prefix '{}'.", bucketName, this.options.prefix);
		this._initialize();
	}

	async _initializeImpl() {
		const result = await this.bucket.exists();
		if (!result[0]) {
			await this.storage.createBucket(this.bucketName);
		}
	}

	_makePath(bucket, key = "") {
		return this.options.prefix + "/" + bucket + "/" + key;
	}

	_getFile(bucket, key) {
		return this.bucket.file(this._makePath(bucket, key));
	}

	async _isImpl(bucket, key) {
		const file = this._getFile(bucket, key);
		const result = await file.exists();
		return result[0];
	}

	async _readImpl(bucket, key) {
		return this._getFile(bucket, key).createReadStream();
	}

	async _delay(timeMs) {
		return new Promise((resolve) => {
			setTimeout(resolve, timeMs);
		});
	}

	async _waitUntilExists(bucket, key, timeoutMs = 10000) {
		let timeMs = 0;
		while (!(await this._isImpl(bucket, key)) && timeMs < timeoutMs) {
			await this._delay(1000);
			timeMs += 1000;
		}
		Exception.assert(await this._isImpl(bucket, key), "File bucket='{}', key='{}' does not exists", bucket, key);
	}

	async _writeImpl(bucket, key, readStream) {
		const file = this._getFile(bucket, key);
		let writeStream = file.createWriteStream();
		await copyStream(writeStream, readStream);
		await this._waitUntilExists(bucket, key);
	}

	async _deleteImpl(bucket, key) {
		const file = this._getFile(bucket, key);
		await file.delete();
	}

	async _listImpl(bucket, maxOrPaging, includeMetadata) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const prefix = this._makePath(bucket);
		const [files, apiResponse] = await this.bucket.getFiles({
			autoPaginate: false,
			prefix: prefix,
			maxResults: paging.max,
			pageToken: paging.page || undefined,
		});
		let result = [];
		for (const file of files) {
			const name = file.metadata.name.slice(prefix.length);
			result.push(
				includeMetadata
					? {
						name: name,
						size: parseInt(file.metadata.size),
						type: file.metadata.contentType,
						modified: file.metadata.updated,
						created: file.metadata.timeCreated,
					  }
					: name
			);
		}
		if (apiResponse && "pageToken" in apiResponse) {
			return new CollectionPaging(result, { page: apiResponse.pageToken, max: paging.max });
		}
		return new CollectionPaging(result);
	}
}
