import ExceptionFactory from "../exception.mjs";
import Event from "#bzd/nodejs/core/event.mjs";

const Exception = ExceptionFactory("websocket");

export default class Rest {
	constructor(schema, options) {
		this.options = Object.assign(
			{
				/// Version of the API to be used.
				version: 1,
				/// Channel to be used as a transportation mean for this REST API
				channel: null,
			},
			options,
		);

		this.schema = schema;
		this.event = new Event({
			ready: { proactive: true },
			error: { proactive: true },
		});
	}

	/// This function waits until the module is ready
	async waitReady() {
		return await this.event.waitUntil("ready");
	}

	getEndpoint(endpoint) {
		return (this.options.host ? this.options.host : "") + "/websocket/v" + this.options.version + endpoint;
	}

	/// Install all given plugins..
	installPlugins(...plugins) {
		for (const plugin of plugins) {
			if (typeof plugin.installWebsocket == "function") {
				plugin.installWebsocket(this);
			}
		}
		this.event.trigger("ready");
	}

	/// Ensure the websocket is valid according to the API schema.
	_sanityCheck(endpoint) {
		Exception.assert(endpoint in this.schema, "This websocket endpoint is not defined: {}", endpoint);
	}
}
