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

	async request(endpoint, options = {}) {
		const baseOptions =
			typeof this.optionsOrCallback == "function" ? await this.optionsOrCallback(options.args) : this.optionsOrCallback;
		return await HttpClient.request(this.url + endpoint, Object.assign({}, baseOptions, options));
	}

	async get(endpoint, options = {}) {
		options["method"] = "get";
		return await this.request(endpoint, options);
	}

	async post(endpoint, options = {}) {
		options["method"] = "post";
		return await this.request(endpoint, options);
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
	static async request(url, options = {}) {
		options = Object.assign(
			{
				/**
				 * Includes all outputs (in this order): data, headers, status code
				 */
				includeAll: false,
				/**
				 * Throw if the response status code is not within the 2xx boundaries
				 */
				throwOnResponseError: true,
				/**
				 * Maximum request timeout in ms
				 */
				timeoutMs: 60 * 1000,
			},
			options,
		);

		// Handle queries
		if ("query" in options) {
			const query = Object.keys(options.query)
				.filter((key) => options.query[key] !== undefined)
				.map((key) => key + "=" + encodeURIComponent(options.query[key]))
				.join("&");
			url += query ? "?" + query : "";
		}

		let headers = {};
		let data = undefined;

		// Generate body
		if ("json" in options) {
			headers["Content-Type"] = "application/json";
			data = JSON.stringify(options.json);
		}
		else if (typeof options.data != "undefined") {
			data = options.data;
		}

		// Add the content length
		if (typeof data == "string") {
			headers["Content-Length"] = data.length;
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
				headers["Authorization"] = "token " + auth.token;
				break;
			default:
				Exception.unreachable("Unsupported authentication type '{}'", auth.type);
			}
		}

		// Sanity checks
		const method = String(options.method).toLowerCase();

		// Handle expected type
		switch (options.expect) {
		case "json":
			headers["Accept"] = "application/json";
			break;
		}

		Object.assign(headers, options.headers);

		let request = (await import("./client/backend.mjs")).default;

		Log.debug("{} {} (headers: {:j})", method, url, headers);
		Log.trace("(body: {})", data);

		const result = await request(url, {
			method: method,
			headers: headers,
			data: data,
			expect: options.expect,
			timeoutMs: options.timeoutMs,
		});

		if (options.throwOnResponseError && (result.code < 200 || result.code > 299)) {
			throw new HttpClientException(
				result.code,
				result.data,
				"Request to '{}' responded with: {}: {}",
				url,
				result.code,
				result.data,
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

		if (options.includeAll) {
			return [dataParsed, result.headers, result.code];
		}
		return dataParsed;
	}

	static async get(url, options = {}) {
		options["method"] = "get";
		return await HttpClient.request(url, options);
	}

	static async post(url, options = {}) {
		options["method"] = "post";
		return await HttpClient.request(url, options);
	}
}
