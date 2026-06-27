import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { Storage } from "#bzd/nodejs/db/storage/storage.js";
import Cache2 from "#bzd/nodejs/core/cache2.js";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.js";

const Exception = ExceptionFactory("plugin");

export default class PluginBase {
	constructor(volume, options, provider, endpoints, extensions = []) {
		this.volume = volume;
		this.options = options;
		this.storage = null;
		this.cache = (options["cache"] || new Cache2("cache")).getAccessor(volume);
		this.statistics = options["statistics"] || new StatisticsProvider();
		this.locks = options["locks"] || null;

		Exception.assert(this.locks, "Missing locks implementation for '{}'.", volume);

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
