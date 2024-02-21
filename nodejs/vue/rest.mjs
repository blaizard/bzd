import Rest from "../core/rest/client.mjs";

export default {
	install(app, options) {
		const rest = new Rest(options.schema, options);
		rest.installPlugins(...options.plugins).catch((e) => console.error(e));
		app.config.globalProperties.$rest = rest;
		app.provide("$rest", rest);
	},
};
