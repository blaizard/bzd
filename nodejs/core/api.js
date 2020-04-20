"use strict";

import Fetch from "./fetch.js";
import ExceptionFactory from "./exception.js";

const Exception = ExceptionFactory("api");

export default class API {
	constructor(definition, /*version = 1*/) {
		this.definition = definition;
		this.endpointPre = "/api/v1";
	}

	_makePath(endpoint) {
		return this.endpointPre + endpoint;
	}

	/**
     * Ensure the requestis valid according to the API definition.
     */
	_sanityCheck(method, endpoint) {
		Exception.assert(endpoint in this.definition, "This endpoint is not defined: {}", endpoint);
		Exception.assert(method in this.definition[endpoint], "The method '{}' is not valid for endpoint '{}'", method, endpoint);
	}

	async request(method, endpoint, /*options = {}*/) {
		this._sanityCheck(method, endpoint);
		const responseOptions = this.definition[endpoint][method].response || {};

		// Build the options
		let fetchOptions = {
			method: method
		};
		if ("type" in responseOptions) {
			fetchOptions.expect = responseOptions.type;
		}

		return await Fetch.request(this._makePath(endpoint), fetchOptions);
	}

	/**
     * Register a callback to handle a request
     */
	handle(web, method, endpoint, callback, /*options = {}*/) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.definition[endpoint][method].request || {};
		const responseOptions = this.definition[endpoint][method].response || {};

		// Build the web options
		let webOptions = {};
		if ("type" in requestOptions) {
			webOptions.type = [requestOptions.type];
		}

		// Create a wrapper to the callback
		const callbackWrapper = async function(request, response) {
			try {
				const result = await callback.call(this, request, response);
				switch (responseOptions.type) {
				case "json":
					Exception.assert(typeof result == "object", "{} {}: callback result must be a json object.", method, endpoint);
					response.json(result);
					break;
				}
			}
			catch (e) {
				Exception.print("Exception Guard", Exception.fromError(e));
				response.status(500).send(e.message);
			}
		};

		web.addRoute(method, this._makePath(endpoint), callbackWrapper, webOptions);
	}
}
