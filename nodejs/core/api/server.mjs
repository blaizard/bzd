"use strict";

import Base from "./base.mjs";
import ExceptionFactory from "../exception.mjs";
import LogFactory from "../log.mjs";

const Exception = ExceptionFactory("api", "server");
const Log = LogFactory("api", "server");

class Context {

	constructor(request, response) {
		this.request = request;
		this.response = response;
		this.debug = {};
		this.manualResponse = false;
	}

	setHeader(key, value) {
		this.response.setHeader(key, value);
	}

	setCookie(name, value, options) {
		options = Object.assign({
			maxAge: 7 * 24 * 60 * 60 * 1000, // in ms
			httpOnly: false
		}, options);
		this.response.cookie(name, value, options);
	}

	getCookie(name, defaultValue) {
		return (name in this.request.cookies) ? this.request.cookies[name] : defaultValue;
	}

	deleteCookie(name) {
		this.response.cookie(name, undefined, {
			maxAge: 0
		});
	}

	addDebug(name, data) {
		this.debug[name] = data;
	}

	setStatus(code, message = null) {
		this.response.status(code);
		if (message) {
			this.response.send(message);
		}
		else {
			this.response.end();
		}
		this.manualResponse = true;
	}
};

export default class APIServer extends Base {

	constructor(schema, options) {
		super(schema, options);
	}

	handleAuthentication(web) {
		Exception.assert(this.options.authentication, "Authentication must be set.");
		this.options.authentication.installAPI(this, web);
	}

    /**
     * Register a callback to handle a request
     */
	handle(web, method, endpoint, callback, /*options = {}*/) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		let authentication = null;
		if (this.schema[endpoint][method].authentication) {
			Exception.assert(this.options.authentication, "This route has authentication requirement but no authentication object was specified.");
			authentication = this.options.authentication;
		}

		// Build the web options
		let webOptions = {};
		if ("type" in requestOptions) {
			webOptions.type = [requestOptions.type];
		}

		// Create a wrapper to the callback
		const callbackWrapper = async function(request, response) {

			let context = new Context(request, response);

			try {

				// Check if this is a request that needs authentication
				let authenticationData = { uid: null };
				if (authentication) {
					if (!await authentication.verify(request, (uid) => { authenticationData.uid = uid; return true; })) {
						return context.setStatus(401, "Unauthorized");
					}
				}

				let data = {};
				switch (requestOptions.type) {
				case "json":
					data = request.body.data;
					break;
				case "query":
					data = request.query;
					break;
				case "upload":
					data = Object.assign({}, request.query || {}, {
						files: Object.keys(request.files || {}).map((key) => request.files[key].path)
					});
					break;
				}

				// Add any params to the data (if any)
				Object.assign(data, request.params);

				// Add debug information
				context.addDebug("data", data);
				context.addDebug("uid", authenticationData.uid);

				const result = await callback.call(context, data, authenticationData.uid);
				if (!context.manualResponse) {
					switch (responseOptions.type) {
					case "json":
						Exception.assert(typeof result == "object", "{} {}: callback result must be a json object.", method, endpoint);
						response.json(result);
						break;
					case "file":
						if (typeof result == "string") {
							response.sendFile(result);
						}
						else if ("pipe" in result) {
							await (new Promise((resolve, reject) => {
								result.on("error", reject)
									.on("end", resolve)
									.on("finish", resolve)
									.pipe(response);
							}));
							response.end();
						}
						else {
							Exception.unreachable("{} {}: callback result is not of a supported format.", method, endpoint);
						}
						break;
					case "raw":
						response.status(200).send(result);
						break;
					default:
						response.sendStatus(200);
					}
				}
			}
			catch (e) {
				Exception.print("Exception Guard");
				Exception.print(Exception.fromError(e));
				Log.error("Context: {:j}", context.debug);
				response.status(500).send(e.message);
			}
		};

		// Update the endpoint to support variables
		// Only supports simple syntax:
		// -> /users/{userId}/books/{bookId} -> /users/:userId/books/:bookId
		let regexpr = /{([^}:]+)}/;
		const updatedEndpoint = endpoint.replace(regexpr, ":$1");

		web.addRoute(method, this._makePath(updatedEndpoint), callbackWrapper, webOptions);
	}
}
