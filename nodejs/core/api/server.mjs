import Url from "url";

import ExceptionFactory from "../exception.mjs";
import LogFactory from "../log.mjs";
import Validation from "../validation.mjs";
import { HttpServerContext, HttpError } from "#bzd/nodejs/core/http/server_context.mjs";

import Base from "./base.mjs";

const Exception = ExceptionFactory("api", "server");
const Log = LogFactory("api", "server");

class APIServerContext extends HttpServerContext {
	constructor(context, api) {
		super(context.request, context.response);
		this.api = api;
		this.debug = {};
		this.manualResponse = false;
	}

	getEndpoint(endpoint) {
		Exception.assert(this.api, "API is not set.");
		return new Url.URL(this.api.getEndpoint(endpoint), this.getHost()).href;
	}

	addDebug(name, data) {
		this.debug[name] = data;
	}

	sendStatus(code, message = null) {
		super.sendStatus(code, message);
		this.manualResponse = true;
	}
}

export default class APIServer extends Base {
	constructor(schema, options) {
		super(schema, options);
		this._installPlugins()
			.then(() => this.event.trigger("ready"))
			.catch((e) => this.event.trigger("error", e));
	}

	/// Register a callback to handle a request
	handle(method, endpoint, callback /*, options = {}*/) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		let authentication = null;
		if (this.schema[endpoint][method].authentication) {
			Exception.assert(
				this.options.authentication,
				"This route has authentication requirement but no authentication object was specified.",
			);
			authentication = this.options.authentication;
		}

		Log.debug(
			"Installing handler for {} {}{}",
			method,
			endpoint,
			authentication === null ? "" : " with authentication",
		);

		// Build the web options
		let webOptions = {};
		if ("type" in requestOptions) {
			webOptions.type = [requestOptions.type];
		}

		// Create a wrapper to the callback
		const handler = async (serverContext) => {
			const context = new APIServerContext(serverContext, this);

			try {
				// Check if this is a request that needs authentication
				let authenticationData = { user: null };
				if (authentication) {
					// Check if user is authorized
					let isAuthorized = await authentication.verify(context, (user) => {
						authenticationData.user = user;
						return true;
					});
					// Check if use has any of the roles
					if (isAuthorized) {
						const authenticationSchema = this.schema[endpoint][method].authentication;
						if (typeof authenticationSchema == "string" || Array.isArray(authenticationSchema)) {
							isAuthorized &= authenticationData.user.matchAnyRoles(authenticationSchema);
						}
					}
					if (!isAuthorized) {
						return context.sendStatus(401, "Unauthorized");
					}
				}

				let data = {};
				switch (requestOptions.type) {
					case "raw":
						data["raw"] = context.getBody();
						break;
					case "json":
						data = context.getBody();
						break;
					case "query":
						data = context.getQueries();
						break;
					case "upload":
						data = Object.assign({}, context.getQueries() || {}, {
							files: context.getFiles(),
						});
						break;
				}
				// Add any params to the data (if any)
				Object.assign(data, context.getParams());

				// Add debug information
				context.addDebug("data", data);
				context.addDebug("user", authenticationData.user);

				if ("validation" in requestOptions) {
					Exception.assert(
						["json", "query"].includes(requestOptions.type),
						"{} {}: validation is not available for {}.",
						method,
						endpoint,
						requestOptions.type,
					);
					const validation = new Validation(requestOptions.validation);
					validation.validate(data, {
						all: true,
					});
				}

				const result = await callback.call(context, data, authenticationData.user);

				if ("validation" in responseOptions) {
					Exception.assert(
						responseOptions.type == "json",
						"{} {}: validation is only available for json type.",
						method,
						endpoint,
					);
					const validation = new Validation(responseOptions.validation);
					validation.validate(result, {
						all: true,
					});
				}

				if (!context.manualResponse) {
					context.setStatus(200);
					switch (responseOptions.type) {
						case "json":
							context.sendJson(result);
							break;
						case "stream":
						case "file":
							await context.sendStream(result);
							break;
						case "raw":
							context.send(result);
							break;
						default:
							context.send();
					}
				}
			} catch (e) {
				if (e instanceof HttpError) {
					context.sendStatus(e.code, e.message);
				} else {
					Exception.print("Exception Guard");
					// The formatting string is important to ensure that error message is not
					// considered as a formatting string.
					Exception.print("{}", Exception.fromError(e));
					Log.error("Context for '{}' {}: {:j}", method, endpoint, context.debug);
					context.sendStatus(500, e.message);
				}
			}
		};

		Exception.assert(this.options.channel, "Channel is missing");
		this.options.channel.addRoute(method, this.getEndpoint(endpoint), handler, webOptions);
	}
}
