import Url from "url";

import { ExceptionFactory, ExceptionPrecondition } from "../exception.mjs";
import LogFactory from "../log.mjs";
import Validation from "../validation.mjs";
import { HttpError } from "#bzd/nodejs/core/http/server_context.mjs";
import { extendObject } from "#bzd/nodejs/utils/object.mjs";

import Base from "./base.mjs";

const Exception = ExceptionFactory("rest", "server");
const Log = LogFactory("rest", "server");

class RestServerContext {
	constructor(context, rest) {
		this.context = context;
		this.rest = rest;
		this.debug = {};
		this.manualResponse = false;
	}

	getEndpoint(endpoint) {
		Exception.assert(this.rest, "Rest is not set.");
		return new Url.URL(this.rest.getEndpoint(endpoint), this.context.getHost()).href;
	}

	addDebug(name, data) {
		this.debug[name] = data;
	}

	sendStatus(...args) {
		this.context.sendStatus(...args);
		this.manualResponse = true;
	}
}

export default class RestServer extends Base {
	constructor(schema, options) {
		super(schema, options);
		this.initCORS();
	}

	initCORS() {
		// Install the CORS preflight endpoints.
		for (const endpoint in this.schema) {
			const cors = Object.entries(this.schema[endpoint]).reduce((cors, [method, options]) => {
				if (options.cors) {
					cors.push(method);
				}
				return cors;
			}, []);
			if (cors) {
				this.options.channel.addRoute("options", this.getEndpoint(endpoint), (serverContext) => {
					serverContext.setHeader("Access-Control-Allow-Origin", "*");
					serverContext.setHeader("Access-Control-Allow-Methods", [...cors, "options"].join(", "));
					serverContext.setHeader("Access-Control-Allow-Headers", "Origin, Content-Type, Accept, Authorization");
					serverContext.sendStatus(200, "OK");
				});
			}
		}
	}

	/// Login a user based on his UID.
	async loginWithUID(context, uid, identifier, persistent) {
		Exception.assert(this.options.authentication, "Authentication is not enabled.");
		return await this.options.authentication.loginWithUID(context, uid, identifier, persistent);
	}

	/// Register a websocket
	handleWebsocket(endpoint, callback) {
		Exception.assert(this.options.channel, "Channel is missing");
		this.options.channel.addRouteWebsocket(this.getEndpoint(endpoint), (ws) => {
			callback(ws);
		});
	}

	/// Register a callback to handle a request
	handle(method, endpoint, callback /*, options = {}*/) {
		this._sanityCheck(method, endpoint);
		const endpointOptions = this.schema[endpoint][method] || {};
		const requestOptions = endpointOptions.request || {};
		const responseOptions = endpointOptions.response || {};

		let authentication = null;
		if (endpointOptions.authentication) {
			Exception.assert(
				this.options.authentication,
				"This route '{}' with method '{}' has authentication requirement but no authentication object was specified.",
				endpoint,
				method,
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
			const restContext = new RestServerContext(serverContext, this);
			const context = extendObject(serverContext, restContext);

			try {
				// Set the CORS access control before the authorization check, because the latter can fail
				// and these headers need to be there regardless.
				if (endpointOptions.cors) {
					context.setHeader("Access-Control-Allow-Origin", "*");
					context.setHeader("Access-Control-Allow-Methods", method + ", options");
					context.setHeader("Access-Control-Allow-Headers", "Origin, Content-Type, Accept, Authorization");
				}

				// Check if this is a request that needs authentication
				let authenticationData = { session: null };
				if (authentication) {
					// Check if session is authorized
					const authenticationSchema = endpointOptions.authentication;
					authenticationData.session = await authentication.verify(
						context,
						Array.isArray(authenticationSchema) ? authenticationSchema : null,
					);
					if (typeof authenticationSchema == "string") {
						Exception.assert(
							authenticationSchema == "optional",
							"Only 'optional' is a valid option, not '{}'.",
							authenticationSchema,
						);
						// Ignore validation.
					} else if (!authenticationData.session) {
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
				context.addDebug("session", authenticationData.session);

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

				if ("scopes" in requestOptions) {
					Exception.assert(authentication, "'scopes' can only be set with authentication.");
					Exception.assertResult(authenticationData.session.getScopes().checkDictionary(data, requestOptions.scopes));
				}

				let result = await callback.call(context, data, authenticationData.session);

				// Add debug information
				context.addDebug("result", result);

				if ("validation" in responseOptions) {
					Exception.assert(
						responseOptions.type == "json",
						"{} {}: validation is only available for json type.",
						method,
						endpoint,
					);
					Exception.assert(result !== null, "A response that expects validation cannot return null.");
					Exception.assert(
						typeof result === "object",
						"A response that expects validation must be a dictionary, instead: {:j}",
						result,
					);
					const validation = new Validation(responseOptions.validation);
					validation.validate(result, {
						all: true,
					});
				}

				if ("scopes" in responseOptions) {
					Exception.assert(authentication, "'scopes' can only be set with authentication.");
					result = authenticationData.session.getScopes().filterDictionary(result, responseOptions.scopes);
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
					e.send(context);
				} else if (e instanceof ExceptionPrecondition) {
					context.sendStatus(400, e.message);
				} else {
					Exception.print("Exception Guard");
					// The formatting string is important to ensure that error message is not
					// considered as a formatting string.
					Exception.print("{}", Exception.fromError(e));
					Log.error("Context for '{}' {}: {:j}", method, this.getEndpoint(endpoint), context.debug);
					context.sendStatus(500, e.message);
				}
			}
		};

		Exception.assert(this.options.channel, "Channel is missing");
		this.options.channel.addRoute(method, this.getEndpoint(endpoint), handler, webOptions);
	}
}
