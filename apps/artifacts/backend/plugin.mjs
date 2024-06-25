import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Storage from "#bzd/nodejs/db/storage/storage.mjs";

const Exception = ExceptionFactory("plugin");

export default class PluginBase {
	constructor(volume, options) {
		this.volume = volume;
		this.options = options;
		this.storage = null;
	}

	setStorage(storage) {
		Exception.assert(storage instanceof Storage, "The storage must be of type Storage: {}", this);
		this.storage = storage;
	}

	resetStorage() {
		this.storage = null;
	}

	getStorage() {
		Exception.assert(this.storage, "No storage associated with this plugin: {}", this);
		return this.storage;
	}
}
