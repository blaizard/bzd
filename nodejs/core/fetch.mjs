

import { base64Encode } from "./crypto.mjs";
import ExceptionFactory from "./exception.mjs";
import LogFactory from "./log.mjs";

const Exception = ExceptionFactory("fetch");
const Log = LogFactory("fetch");

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

		let headers = {};
		let body = undefined;

		// Generate body
		if (typeof options.data == "object") {
			headers["Content-Type"] = "application/json";
			body = JSON.stringify(options.data);
		}
		else if (typeof options.data != "undefined") {
			body = options.data;
		}

		// Add the content length
		if (typeof body == "string") {
			headers["Content-Length"] = body.length;
		}

		// Update headers with authentication information
		if ("authentication" in options) {
			const auth = options.authentication;
			switch (auth.type) {
			case "basic":
				{
					const base64 = await base64Encode(auth.username + ":" + auth.password);
					headers["Authorization"] = "basic " + base64;
				}
				break;
			case "bearer":
				headers["Authorization"] = "bearer " + auth.token;
				break;
			case "token":
				headers["Authorization"] = "token " + auth.token;
				break;
			default:
				Exception.unreachable("Unsupported authentication type '{}'", auth.type);
			}
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

		let request = null;
		if (process.env.BZD_RULE === "nodejs_web") {
			request = (await import(/* webpackMode: "eager" */"./impl/fetch/window.fetch.mjs")).default;
		}
		else {
			request = (await import(/* webpackMode: "eager" */"./impl/fetch/node.http.mjs")).default;
		}

		Log.debug("{} {} (headers: {:j}) (body: {:j})", method, url, Object.assign(headers, options.headers), body);

		const data = await request(url, {
			method: method,
			headers: Object.assign(headers, options.headers),
			body: body,
			expect: options.expect
		});

		switch (options.expect) {
		case "json":
			return JSON.parse(data);
		default:
			return data;
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
