import Base from "./storage.mjs";
import LogFactory from "../../core/log.mjs";
import CloudAPI from "@google-cloud/storage";
import { copy as copyStream } from "../../core/stream.mjs";

const Storage = CloudAPI.Storage;
const Log = LogFactory("db", "storage", "google-cloud-storage");
/**
 * File storage module
 */
export default class StorageGoogleCloudStorage extends Base {
	constructor(bucketName, options) {
		super();

		this.options = Object.assign({}, options);
		this.storage = new Storage();
		this.bucketName = bucketName;
		this.bucket = this.storage.bucket(this.bucketName);

		Log.info("Using Google cloud storage DB with bucket '{}'.", bucketName);
		this._initialize();
	}

	async _initializeImpl() {
		const result = await this.bucket.exists();
		if (!result[0]) {
			await this.storage.createBucket(this.bucketName);
		}
	}

	_makePath(bucket, key) {
		return bucket + "/" + key;
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

	async _writeImpl(bucket, key, readStream) {
		const file = this._getFile(bucket, key);
		let writeStream = file.createWriteStream();

		return copyStream(writeStream, readStream);
	}

	async _deleteImpl(bucket, key) {
		const file = this._getFile(bucket, key);
		await file.delete();
	}

	async _listImpl(bucket) {
		const prefix = bucket + "/";
		const [files] = await this.bucket.getFiles({
			prefix: prefix,
		});
		return files.map((file) => file.metadata.name.slice(prefix.length));
	}
}
