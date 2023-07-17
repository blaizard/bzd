import Http from "http";
import Url from "url";

import HttpClient from "./client.mjs";
import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";

const Log = LogFactory("http", "proxy");
const Exception = ExceptionFactory("http", "proxy");

export default class HttpProxy {
	constructor(target, port, options) {
		this.target = new Url.URL(target);
		this.port = port;
		this.options = Object.assign(
			{
				// Maximum request timeout
				timeoutMs: 60 * 1000,
				// Send X-Forward-* headers
				xForward: false,
				beforeRequest: (/*request, response*/) => {},
				afterRequest: (/*request, response*/) => {},
			},
			options,
		);
		this.server = null;

		Exception.assert(
			["http:", "https:"].includes(this.target.protocol),
			"Protocol not supported '{}'",
			this.target.protocol,
		);
	}

	async start() {
		this.server = Http.createServer(async (request, response) => {
			request.setTimeout(this.options.timeoutMs);
			response.setTimeout(this.options.timeoutMs);

			request.on("aborted", () => {
				Log.info("ABORT request {}/{}", this.target.hostname, request.url);
			});

			response.on("aborted", () => {
				Log.info("ABORT response {}/{}", this.target.hostname, request.url);
			});

			if (this.options.xForward) {
				const appendHeader = (name, newValue) => {
					request.headers[name] = (request.headers[name] ? request.headers[name] + "," : "") + newValue;
				};
				appendHeader("x-forwarded-for", request.connection.remoteAddress || request.socket.remoteAddress);
				appendHeader("x-forwarded-port", this.port);
				appendHeader("x-forwarded-proto", "http");
				request.headers["x-forwarded-host"] = request.headers["x-forwarded-host"] || request.headers["host"] || "";
			}

			// Logger for request
			Log.debug("> {} {}", request.method, request.url);
			Log.trace("> {:j}", request.headers);

			await this.options.beforeRequest(request, response);

			// Response has already been sent by the previous call, do not continue
			if (response.writableEnded) {
				return;
			}

			// Delete host header as it will be set afterward
			delete request.headers.host;

			const proxyUrl = new URL(request.url, this.target);
			let proxyResponse = {};
			[proxyResponse.data, proxyResponse.headers, proxyResponse.statusCode] = await HttpClient.request(proxyUrl.href, {
				method: request.method,
				headers: request.headers,
				data: request,
				expect: "stream",
				timeoutMs: this.options.timeoutMs,
				includeAll: true,
				throwOnResponseError: false,
			});

			await this.options.afterRequest(request, proxyResponse);

			response.writeHead(proxyResponse.statusCode, proxyResponse.headers);

			// Pipe the response directly
			proxyResponse.data.pipe(response, {
				end: true,
			});

			// Logger for response
			Log.debug("< {} {} {}", proxyResponse.statusCode, request.method, request.url);
			Log.trace("< {:j}", proxyResponse.headers);
		});
		this.server.setTimeout(this.options.timeoutMs, () => {
			Log.error("TIMEOUT server {} ({} ms)", this.target.hostname, this.server.timeout);
		});

		this.server.keepAliveTimeout = this.options.timeoutMs;
		this.server.headersTimeout = this.options.timeoutMs;

		this.server.on("clientError", (e, socket) => {
			Log.error("{}: {:j}", this.target.hostname, e);
			socket.end("HTTP/1.1 400 Bad Request\r\n\r\n");
		});

		return new Promise((resolve, reject) => {
			this.server
				.listen(this.port)
				.once("listening", () => {
					Log.info("Deployed HTTP Proxy for '{}' listening on port {}", this.target.href, this.port);
					resolve();
				})
				.once("error", reject);
		});
	}

	_closeServer() {
		return new Promise((resolve) => {
			this.server.close(() => {
				resolve();
			});
		});
	}

	async stop() {
		await this._closeServer();
	}
}
