"use strict";

import ExceptionFactory from "./exception.js";

const Exception = ExceptionFactory("fetch");

/**
 * Fetch data from HTTP endpoint.
 */
export default class Fetch {
	static async request(url, options = {}) {

		// Handle queries
		if ("query" in options) {
			const query = Object.keys(options.query).map((key) => key + "=" + encodeURIComponent(options.query[key])).join("&");
			url += ((query) ? ("?" + query) : "");
		}

		let headers = options.headers || {};
		let body = undefined;

		// Generate body
		if (options.data instanceof FormData) {
			headers["Content-Type"] = "application/x-www-form-urlencoded";
			body = JSON.stringify(options.data);
		}
		else if (typeof options.data == "object") {
			headers["Content-Type"] = "application/json";
			body = JSON.stringify(options.data);
		}
		else if (typeof options.data != "undefined") {
			body = options.data;
		}

		// Sanity checks
		const method = String(options.method).toLowerCase();
		Exception.assert(method in {"get":1, "post":1, "head":1, "put":1, "delete":1, "connect":1, "options":1, "trace":1, "patch":1}, "Method '{}' is not supported", method);
		Exception.assert(!(body && (method in {"get":1})), "Body cannot be set with method '{}', this is against recommendation in the HTTP/1.1 spec, section 4.3", method);

		// Handle expected type
		switch (options.expect) {
		case "json":
			headers["Accept"] = "application/json";
			break;
		}

		const response = await window.fetch(url, {
			method: method,
			body: body,
			headers: headers
		});

		Exception.assert(response.ok, async () => { return (await response.text()) || response.statusText; });

		switch (options.expect) {
		case "json":
			return await response.json();
		default:
			return await response.text();
		}
	}

	static async get(url, options = {}) {
		options["method"] = "get";
		return await this.request(url, options);
	}

	static async post(url, options = {}) {
		options["method"] = "post";
		return await this.request(url, options);
	}
}
