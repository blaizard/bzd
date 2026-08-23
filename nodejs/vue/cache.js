import { reactive } from "vue";
import Cache from "../core/cache.js";
import ExceptionFactory from "../core/exception.js";

const Exception = ExceptionFactory("cache");

export default {
	install(app, options = {}) {
		let cache = new Cache("vue-cache");
		for (const collection in options) {
			const option = options[collection];
			Exception.assert("cache" in option, "Missing 'cache' key");
			cache.register(collection, option.cache, option.options);
		}

		const content = reactive({});

		const getReactive = (collection, key) => {
			// If data does not exists, create it
			if (!(collection in content)) {
				content[collection] = {};
			}
			if (!(key in content[collection])) {
				content[collection][key] = options[collection].default || "";
			}

			cache.get(collection, key).then((value) => {
				content[collection][key] = value;
			});

			return content[collection][key];
		};

		const get = async (collection, key) => {
			return await cache.get(collection, key);
		};

		const invalid = (collection, key) => {
			cache.setDirty(collection, key);
			if (collection in content) {
				content[collection][key] = options[collection].loading || "";
			}
		};

		app.config.globalProperties.$cache = {
			getReactive,
			get,
			invalid,
		};
	},
};
