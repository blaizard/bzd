import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { toString } from "#bzd/nodejs/core/stream.mjs";

const Exception = ExceptionFactory("http", "server", "context", "mock");

export default class MockServerContext {
	constructor() {
		this.request = {
			path: null,
			method: "GET",
			queries: {},
			params: {},
			headers: {},
			cookies: {},
			files: [],
			type: "default",
			data: "",
		};
		this.response = {
			status: null,
			data: null,
			redirect: null,
			end: false,
			headers: {},
		};
	}

	static make(request = {}) {
		const context = new MockServerContext();
		Object.assign(context.request, request);
		return context;
	}

	withPath(url) {
		const parsedUrl = new URL(url, "http://dummy");
		this.request.path = parsedUrl.pathname;
		Object.assign(this.request.queries, Object.fromEntries(parsedUrl.searchParams.entries()));
		return this;
	}

	withParams(params) {
		Object.assign(this.request.params, params);
	}

	getHost() {
		return "https://dummy";
	}

	getPath() {
		return this.request.path;
	}

	hasHeader(name) {
		return name in this.request.headers;
	}

	getHeaders() {
		const headers = Object.assign(
			{
				host: this.getHost(),
				connection: "close",
				"content-length": String(this.request.data).length,
			},
			this.request.headers,
		);
		// Set key to lower case.
		return Object.fromEntries(Object.entries(headers).map(([k, v]) => [k.toLowerCase(), v]));
	}

	getHeader(name, defaultValue = null) {
		const headers = this.getHeaders();
		return name in headers ? headers[name] : defaultValue;
	}

	setHeader(key, value) {
		this.response.headers[key] = value;
		return this;
	}

	setCookie(name, value, options = {}) {
		this.request.cookies[name] = value;
		return this;
	}

	getCookie(name, defaultValue = null) {
		return name in this.request.cookies ? this.request.cookies[name] : defaultValue;
	}

	deleteCookie(name) {
		delete this.request.cookies[name];
	}

	setStatus(code) {
		Exception.assert(!this.response.end, "Response is already sent.");
		Exception.assert(this.response.status === null, "Status already set.");
		this.response.status = code;
	}

	// todo
	getBody() {
		switch (this.request.type) {
			case "raw":
				let content = this.request.method + " " + this.request.path + " HTTP/1.1\r\n";
				content += Object.entries(this.getHeaders())
					.map(([key, value]) => key + ": " + value + "\r\n")
					.join("");
				content += "\r\n\r\n";
				content += this.request.data;
				return content;
			case "default":
				return this.request.data;
		}
	}

	getQueries() {
		return this.request.queries;
	}

	getQuery(name, defaultValue = null, cast = (v) => v) {
		return name in this.request.queries ? cast(this.request.queries[name]) : defaultValue;
	}

	getFiles() {
		return this.request.files;
	}

	getParams() {
		return this.request.params;
	}

	getParam(name, defaultValue = null) {
		return name in this.request.params ? this.request.params[name] : defaultValue;
	}

	sendStatus(code, message = null) {
		this.setStatus(code);
		this.send(message);
	}

	sendJson(data) {
		Exception.assert(
			typeof data == "object",
			"{}: data must be a json object, instead received: {}",
			typeof data,
			data,
		);
		this.send(data);
	}

	async sendStream(data) {
		if (typeof data == "string") {
			this.send(data);
		} else if ("pipe" in data) {
			const buffer = await toString(data);
			this.send(buffer);
		} else {
			Exception.unreachable("{} {}: callback result is not of a supported format.", method, endpoint);
		}
	}

	send(data = null) {
		Exception.assert(!this.response.end, "Response is already sent.");
		Exception.assert(this.response.data === null, "Response data is set.");
		this.response.data = data;
		this.response.end = true;
	}

	redirect(url) {
		this.response.redirect = url;
		this.response.end = true;
	}

	httpError(code, message) {
		return new HttpError(code, message);
	}
}
