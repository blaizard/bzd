import ExceptionFactory from "../exception.mjs";
import { pipeline, finished } from "stream/promises";
import Busboy from "busboy";
import { toString } from "#bzd/nodejs/core/stream.mjs";

import { Writable } from "stream";

const Exception = ExceptionFactory("http", "server", "context");

/// Handler for http errors.
export class HttpError extends Error {
	constructor(code, message, headers = {}) {
		super(message);
		this.code = code;
		this.headers = headers;
	}

	send(context) {
		for (const [name, value] of Object.entries(this.headers)) {
			context.setHeader(name, value);
		}
		context.sendStatus(this.code, this.message);
	}
}

/// Abstraction of the request/response pair of the server.
export class HttpServerContext {
	constructor(request, response) {
		this.request = request;
		this.response = response;
	}

	getHost() {
		const protocol = this.request.headers["x-forwarded-proto"] || this.request.protocol;
		return protocol + "://" + this.request.get("host");
	}

	getPath() {
		return this.request.path;
	}

	hasHeader(name) {
		return name in this.request.headers;
	}

	/// Set the callback to handle file streams
	async processForm(onFile) {
		return new Promise((resolve, reject) => {
			const busboy = Busboy({ headers: this.request.headers });
			const processingTasks = [];
			let inputs = {};
			let inputSerialized = null;
			let blobs = {};
			busboy.on("file", (key, stream, info) => {
				processingTasks.push(
					(async () => {
						const actualFilename = info.filename.startsWith("@bzd-") ? info.filename.split("-", 4)[3] : info.filename;
						try {
							blobs[info.filename] = await onFile(key, stream, {
								name: actualFilename,
								mime: info.mimeType,
							});
						} finally {
							stream.resume();
							await finished(stream);
						}
					})(),
				);
			});
			busboy.on("field", (key, value, info) => {
				if (key == "@bzd-form") {
					Exception.assertPrecondition(inputSerialized === null, "Cannot have 2 entries with key '@bzd-form'.");
					inputSerialized = value;
				} else {
					inputs[key] = value;
				}
			});
			busboy.on("close", () => {
				// Finish processing the I/Os.
				Promise.all(processingTasks)
					.then(() => {
						if (inputSerialized) {
							// At this point the blobs are fully processed.
							Object.assign(
								inputs,
								JSON.parse(inputSerialized, (_, value) => {
									if (value in blobs) {
										return blobs[value];
									}
									return value;
								}),
							);
						}
						resolve(inputs);
					})
					.catch(reject);
			});
			busboy.on("error", (error) => {
				reject(error);
			});
			this.request.pipe(busboy);
		});
	}

	/// Get the header
	getHeader(name, defaultValue = null, allowMultiple = false) {
		const header = this.request.headers[name] ?? null;
		if (header === null) {
			return defaultValue;
		}
		const headerArray = Array.isArray(header) ? header : [header];
		if (allowMultiple) {
			return headerArray;
		}
		return headerArray[0];
	}

	setHeader(key, valueOrValues) {
		this.response.setHeader(key, valueOrValues);
	}

	setCookie(name, value, options = {}) {
		options = Object.assign(
			{
				maxAge: 7 * 24 * 60 * 60 * 1000, // in ms
				httpOnly: false,
			},
			options,
		);
		this.response.cookie(name, value, options);
	}

	getCookie(name, defaultValue = null) {
		return name in this.request.cookies ? this.request.cookies[name] : defaultValue;
	}

	deleteCookie(name) {
		this.response.cookie(name, undefined, {
			maxAge: 0,
		});
	}

	setStatus(code) {
		this.response.status(code);
	}

	getBody() {
		return this.request.body;
	}

	getQueries() {
		return this.request.query;
	}

	getQuery(name, defaultValue = null, cast = (v) => v) {
		return name in this.request.query ? cast(this.request.query[name]) : defaultValue;
	}

	getFiles() {
		return Object.keys(this.request.files || {}).map((key) => this.request.files[key].path);
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
		this.response.json(data);
		this.response.end();
	}

	async sendStream(data) {
		if (typeof data == "string") {
			this.response.sendFile(data);
		} else if ("pipe" in data) {
			await pipeline(data, this.response);
		} else {
			Exception.unreachable("{} {}: callback result is not of a supported format.", method, endpoint);
		}
	}

	send(data = null) {
		if (data != null) {
			this.response.send(data);
		}
		this.response.end();
	}

	redirect(url) {
		this.response.redirect(url);
		this.response.end();
	}

	httpError(...args) {
		return new HttpError(...args);
	}
}

/// Websocket context.
export class WebsocketServerContext {
	constructor(ws, params) {
		this.ws = ws;
		this.params = params;
	}

	getParams() {
		return this.params;
	}

	getParam(name, defaultValue = null) {
		return name in this.params ? this.params[name] : defaultValue;
	}

	send(data) {
		this.ws.send(data);
	}

	read(handler) {
		this.ws.on("message", handler);
	}

	exit(handler) {
		this.ws.on("close", handler);
	}
}
