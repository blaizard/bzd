import ExceptionFactory from "../exception.mjs";
import Event from "#bzd/nodejs/core/event.mjs";

const Exception = ExceptionFactory("api");

export default class API {
	constructor(schema, options) {
		this.options = Object.assign(
			{
				/// The host to be used, if null, the current host is used.
				host: null,
				/// Authentication object to be used with this API.
				authentication: null,
				/// Version of the API to be used.
				version: 1,
				/// Channel to be used as a transportation mean for this API
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

	/**
	 * This function waits until the module is ready
	 */
	async waitReady() {
		return await this.event.waitUntil("ready");
	}

	/**
	 * If authentication is supported
	 */
	isAuthentication() {
		return this.options.authentication !== null;
	}

	getEndpoint(endpoint) {
		return (this.options.host ? this.options.host : "") + "/api/v" + this.options.version + endpoint;
	}

	/// Install all given plugins..
	async installPlugins(...plugins) {
		for (const plugin of plugins) {
			if (typeof plugin.installAPI == "function") {
				await plugin.installAPI(this);
			}
		}
		this.event.trigger("ready");
	}

	/**
	 * Ensure the requestis valid according to the API schema.
	 */
	_sanityCheck(method, endpoint) {
		Exception.assert(endpoint in this.schema, "This endpoint is not defined: {}", endpoint);
		Exception.assert(
			method in this.schema[endpoint],
			"The method '{}' is not valid for endpoint '{}'",
			method,
			endpoint,
		);
	}
}
