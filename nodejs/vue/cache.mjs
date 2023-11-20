import Cache from "../core/cache.mjs";
import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("cache");

function idsToId(...ids) {
	return ids.map((id) => String(id)).join("/") || "default";
}

export default {
	install(Vue, options = {}) {
		let cache = new Cache();
		for (const collection in options) {
			const option = options[collection];
			Exception.assert("cache" in option, "Missing 'cache' key");
			cache.register(collection, option.cache, option.options || {});
		}

		Vue.prototype.$cache = new Vue({
			data: {
				content: {},
			},
			methods: {
				getReactive(collection, ...ids) {
					const id = idsToId(...ids);

					// If data does not exists, create it
					if (!(collection in this.content)) {
						this.content[collection] = {};
					}
					if (!(id in this.content[collection])) {
						this.content[collection][id] = options[collection].default || "";
					}

					cache.get(collection, ...ids).then((value) => {
						this.content[collection][id] = value;
					});

					return this.content[collection][id];
				},
				async get(collection, ...ids) {
					return await cache.get(collection, ...ids);
				},
				invalid(collection, ...ids) {
					const id = idsToId(...ids);
					cache.setDirty(collection, ...ids);
					if (collection in this.content) {
						this.content[collection][id] = options[collection].loading || "";
					}
				},
			},
		});
	},
};
