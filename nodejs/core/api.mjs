"use strict";

import Fetch from "./fetch.mjs";
import ExceptionFactory from "./exception.mjs";

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

	async request(method, endpoint, data) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.definition[endpoint][method].request || {};
		const responseOptions = this.definition[endpoint][method].response || {};

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
					Exception.assert(typeof result == "string", "{} {}: callback result must be a string.", method, endpoint);
					response.sendFile(result);
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
