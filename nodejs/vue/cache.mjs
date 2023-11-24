import { reactive } from "vue";
import Cache from "../core/cache.mjs";
import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("cache");

function idsToId(...ids) {
	return ids.map((id) => String(id)).join("/") || "default";
}

export default {
	install(app, options = {}) {
		let cache = new Cache();
		for (const collection in options) {
			const option = options[collection];
			Exception.assert("cache" in option, "Missing 'cache' key");
			cache.register(collection, option.cache, option.options || {});
		}

		const content = reactive({});

		const getReactive = (collection, ...ids) => {
			const id = idsToId(...ids);

			// If data does not exists, create it
			if (!(collection in content)) {
				content[collection] = {};
			}
			if (!(id in content[collection])) {
				content[collection][id] = options[collection].default || "";
			}

			cache.get(collection, ...ids).then((value) => {
				content[collection][id] = value;
			});

			return content[collection][id];
		};

		const get = async (collection, ...ids) => {
			return await cache.get(collection, ...ids);
		};

		const invalid = (collection, ...ids) => {
			const id = idsToId(...ids);
			cache.setDirty(collection, ...ids);
			if (collection in content) {
				content[collection][id] = options[collection].loading || "";
			}
		};

		app.config.globalProperties.$cache = {
			getReactive,
			get,
			invalid,
		};
	},
};
