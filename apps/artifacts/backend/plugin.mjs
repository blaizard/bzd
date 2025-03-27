import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Storage from "#bzd/nodejs/db/storage/storage.mjs";

const Exception = ExceptionFactory("plugin");

export default class PluginBase {
	constructor(volume, options, provider, endpoints, extensions = []) {
		this.volume = volume;
		this.options = options;
		this.storage = null;

		// Register extensions.
		for (const extension of extensions) {
			extension(this, options, provider, endpoints);
		}
	}

	setStorage(storage) {
		Exception.assert(storage instanceof Storage, "The storage must be of type Storage: {}", storage);
		this.storage = storage;
	}

	resetStorage() {
		this.storage = null;
	}

	getStorage() {
		Exception.assert(
			this.storage,
			"No storage associated with this plugin: {}, maybe the initialization step failed?",
			this.volume,
		);
		return this.storage;
	}
}
