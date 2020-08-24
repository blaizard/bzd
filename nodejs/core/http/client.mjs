import { base64Encode } from "../crypto.mjs";
import ExceptionFactory from "../exception.mjs";
import LogFactory from "../log.mjs";

const Exception = ExceptionFactory("http", "client");
const Log = LogFactory("http", "client");

/**
 * Fetch data from HTTP endpoint.
 */
export class HttpClientFactory {
	constructor(url, optionsOrCallback = {}) {
		this.url = url;
		this.optionsOrCallback = optionsOrCallback;
	}

	async request(endpoint, options = {}, includeAll = false) {
		const baseOptions =
			typeof this.optionsOrCallback == "function" ? await this.optionsOrCallback(options.args) : this.optionsOrCallback;
		return await HttpClient.request(this.url + endpoint, Object.assign({}, baseOptions, options), includeAll);
	}

	async get(endpoint, options = {}, includeAll = false) {
		options["method"] = "get";
		return await this.request(endpoint, options, includeAll);
	}

	async post(endpoint, options = {}, includeAll = false) {
		options["method"] = "post";
		return await this.request(endpoint, options, includeAll);
	}
}

export class HttpClientException extends ExceptionFactory("fetch", "impl") {
	constructor(code, data, ...args) {
		super(...args);
		this.code = code;
		this.data = data;
	}
}

export default class HttpClient {
	static async request(url, options = {}, includeAll = false) {
		// Handle queries
		if ("query" in options) {
			const query = Object.keys(options.query)
				.filter((key) => options.query[key] !== undefined)
				.map((key) => key + "=" + encodeURIComponent(options.query[key]))
				.join("&");
			url += query ? "?" + query : "";
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
			// base64-encoded credentials - RFC 7617
			case "basic":
				{
					// Big 'B' for Basic is important for at least Google GCR
					const base64 = await base64Encode(auth.username + ":" + auth.password);
					headers["Authorization"] = "Basic " + base64;
				}
				break;
				// bearer tokens to access OAuth 2.0-protected resources - RFC 6750
			case "bearer":
				headers["Authorization"] = "Bearer " + auth.token;
				break;
			case "token":
				// Small 't' for token is important for at least Travis CI
				headers["Authorization"] = "token " + auth.token;
				break;
			default:
				Exception.unreachable("Unsupported authentication type '{}'", auth.type);
			}
		}

		// Sanity checks
		const method = String(options.method).toLowerCase();
		Exception.assert(
			method in
				{
					get: 1,
					post: 1,
					head: 1,
					put: 1,
					delete: 1,
					connect: 1,
					options: 1,
					trace: 1,
					patch: 1
				},
			"Method '{}' is not supported",
			method
		);
		Exception.assert(
			!(body && method in { get: 1 }),
			"Body cannot be set with method '{}', this is against recommendation in the HTTP/1.1 spec, section 4.3",
			method
		);

		// Handle expected type
		switch (options.expect) {
		case "json":
			headers["Accept"] = "application/json";
			break;
		}

		let request = null;
		if (process.env.BZD_RULE === "nodejs_web") {
			request = (await import(/* webpackMode: "eager" */ "./client/window.fetch.mjs")).default;
		}
		else {
			request = (await import(/* webpackMode: "eager" */ "./client/node.http.mjs")).default;
		}

		Log.debug("{} {} (headers: {:j}) (body: {:j})", method, url, Object.assign(headers, options.headers), body);

		const result = await request(url, {
			method: method,
			headers: Object.assign(headers, options.headers),
			body: body,
			expect: options.expect
		});

		if (result.code < 200 || result.code > 299) {
			throw new HttpClientException(
				result.code,
				result.data,
				"Request to '{}' responded with: {}: {}",
				url,
				result.code,
				result.data
			);
		}

		const dataParsed = ((data) => {
			switch (options.expect) {
			case "json":
				return data ? JSON.parse(data) : {};
			default:
				return data;
			}
		})(result.data);

		if (includeAll) {
			return [dataParsed, result.headers, result.code];
		}
		return dataParsed;
	}

	static async get(url, options = {}, includeAll = false) {
		options["method"] = "get";
		return await HttpClient.request(url, options, includeAll);
	}

	static async post(url, options = {}, includeAll = false) {
		options["method"] = "post";
		return await HttpClient.request(url, options, includeAll);
	}
}
