import WebsocketClient from "../core/websocket/client.mjs";

export default {
	install(app, options) {
		options = Object.assign(
			{
				plugins: [],
			},
			options,
		);
		const websocket = new WebsocketClient(options.schema, options);
		websocket.installPlugins(...options.plugins);
		app.config.globalProperties.$websocket = websocket;
		app.provide("$websocket", websocket);
	},
};
