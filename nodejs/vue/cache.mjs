import Cache from "../core/cache.mjs";
import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("cache");

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
				get(collection, id = "default") {
					// If data does not exists, create it
					if (!(collection in this.content)) {
						this.$set(this.content, collection, {});
					}
					if (!(id in this.content[collection])) {
						this.$set(this.content[collection], id, options[collection].default || "");
					}

					if (cache.isDirty(collection, id)) {
						(async () => {
							this.content[collection][id] = await cache.get(collection, id);
						})();
					}
					return this.content[collection][id];
				},
				invalid(collection, id = "default") {
					cache.setDirty(collection, id);
					this.content[collection][id] = options[collection].loading || "";
				},
			},
		});
	},
};
