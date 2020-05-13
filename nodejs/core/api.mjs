"use strict";

import Fetch from "./fetch.mjs";
import ExceptionFactory from "./exception.mjs";
import ExceptionFetch from "./impl/fetch/exception.mjs";

const Exception = ExceptionFactory("api");

export default class API {
	constructor(schema, unauthorizedCallback = null, /*version = 1*/) {
		this.schema = schema;
		this.endpointPre = "/api/v1";
		this.token = null;
		this.unauthorizedCallback = unauthorizedCallback;
	}

	_makePath(endpoint) {
		return this.endpointPre + endpoint;
	}

	/**
     * Ensure the requestis valid according to the API schema.
     */
	_sanityCheck(method, endpoint) {
		Exception.assert(endpoint in this.schema, "This endpoint is not defined: {}", endpoint);
		Exception.assert(method in this.schema[endpoint], "The method '{}' is not valid for endpoint '{}'", method, endpoint);
	}

	/**
	 * Register a token with this API
	 */
	setToken(token) {
		this.token = token;
	}

	async request(method, endpoint, data) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		// Check if this is a request that needs authentication
		if (this.schema[endpoint][method].authentication && !this.token) {
			if (this.unauthorizedCallback) {
				return this.unauthorizedCallback();
			}
		}

		// Build the options
		let fetchOptions = {
			method: method
		};
		if ("type" in responseOptions) {
			fetchOptions.expect = responseOptions.type;
		}

		switch (requestOptions.type) {
		case "json":
			Exception.assert(typeof data === "object", "Data must be of type 'object', got '{:j}' instead.", data);
			fetchOptions.data = data;
			break;
		case "query":
			Exception.assert(typeof data === "object", "Data must be of type 'object', got '{:j}' instead.", data);
			fetchOptions.query = data;
			break;
		}

		// Automatically add authentication information to the request
		if (this.token) {
			fetchOptions.authentication = {
				type: "token",
				token: this.token
			}
		}

		try {
			return await Fetch.request(this._makePath(endpoint), fetchOptions);
		}
		catch (e) {
			if (e instanceof ExceptionFetch) {
				if (e.code == 401/*Unauthorized*/ || e.code == 403/*Forbidden*/) {
					if (this.unauthorizedCallback) {
						return this.unauthorizedCallback();
					}
				}
			}
			throw e;
		}
	}

	/**
     * Register a callback to handle a request
     */
	handle(web, method, endpoint, callback, /*options = {}*/) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		// Build the web options
		let webOptions = {};
		if ("type" in requestOptions) {
			webOptions.type = [requestOptions.type];
		}

		// Create a wrapper to the callback
		const callbackWrapper = async function(request, response) {
			try {
				let data = null;
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

				const result = await callback.call(this, data);
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
			catch (e) {
				Exception.print("Exception Guard");
				Exception.print(Exception.fromError(e));
				response.status(500).send(e.message);
			}
		};

		web.addRoute(method, this._makePath(endpoint), callbackWrapper, webOptions);
	}
}
