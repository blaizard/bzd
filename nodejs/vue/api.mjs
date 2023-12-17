import API from "../core/api/client.mjs";

export default {
	install(app, options) {
		const api = new API(options.schema, options);
		api.installPlugins(...options.plugins).catch((e) => console.error(e));
		app.config.globalProperties.$api = api;
		app.provide("$api", api);
	},
};
