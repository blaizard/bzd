import Http from "http";
import Https from "https";
import Url from "url";

import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";

const Log = LogFactory("http", "proxy");
const Exception = ExceptionFactory("http", "proxy");

const debugLevel = 2;

export default class HttpProxy {
	constructor(target, port, options) {
		this.target = new Url.URL(target);
		this.port = port;
		this.options = Object.assign(
			{
				// Maximum request timeout
				timeoutMs: 84600 * 1000,
				// Send X-Forward-* headers
				xForward: true,
				beforeRequest: (/*request, response*/) => {},
				afterRequest: (/*request, response*/) => {}
			},
			options
		);
		this.server = null;

		Exception.assert(
			["http:", "https:"].includes(this.target.protocol),
			"Protocol not supported '{}'",
			this.target.protocol
		);
	}

	/**
	 * Test function to sniff and display insights from the HTTP requests
	 */
	_sniffProxyRequest(req) {
		if (debugLevel == 1) {
			Log.info("> {} {}", req.method, req.url);
		}
		else {
			Log.info("> {} {} {:j}", req.method, req.url, req.headers);
		}
	}
	_sniffProxyResponse(req, response) {
		if (debugLevel == 1) {
			Log.info("< {} {} {}", response.statusCode, req.method, req.url);
		}
		else if (debugLevel == 2) {
			Log.info("< {} {} {} {:j}", response.statusCode, req.method, req.url, response.headers);
		}
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

			request.headers["user-agent"] = "gcr-proxy/0.1 customDomain/127.0.0.1";
			request.headers["accept"] = "*/*";

			if (this.options.xForward) {
				const appendHeader = (name, newValue) => {
					request.headers[name] = (request.headers[name] ? request.headers[name] + "," : "") + newValue;
				};
				appendHeader("x-forwarded-for", request.connection.remoteAddress || request.socket.remoteAddress);
				appendHeader("x-forwarded-port", this.port);
				appendHeader("x-forwarded-proto", "http");
				request.headers["x-forwarded-host"] = request.headers["x-forwarded-host"] || request.headers["host"] || "";
			}

			this._sniffProxyRequest(request);

			await this.options.beforeRequest(request, response);

			// Response has already been sent by the previous call, do not continue
			if (response.writableEnded) {
				return;
			}

			// Delete host header as it will be set afterward
			delete request.headers.host;

			// Protocol use and call the request
			const Protocol = this.target.protocol == "http:" ? Http : Https;
			let proxyRequest = Protocol.request({
				hostname: this.target.hostname,
				port: this.target.port,
				path: request.url,
				method: request.method,
				headers: request.headers,
				timeout: this.options.timeoutMs
			});

			proxyRequest.on("abort", () => {
				Log.info("ABORT {}/{}", this.target.hostname, request.url);
			});

			proxyRequest.on("timeout", () => {
				Log.error("TIMEOUT proxy request {}/{}", this.target.hostname, request.url);
				proxyRequest.abort();
			});

			proxyRequest.on("error", (e) => {
				Log.error("{}/{}: {:j}", this.target.hostname, request.url, e);
			});

			proxyRequest.on("end", () => {
				if (!response.complete) {
					Log.error(
						"{}/{}: The connection was terminated while the message was still being sent",
						this.target.hostname,
						request.url
					);
				}
			});

			proxyRequest.on("response", async (proxyResponse) => {
				await this.options.afterRequest(request, proxyResponse);

				response.writeHead(proxyResponse.statusCode, proxyResponse.headers);

				// Pipe the response directly
				proxyResponse.pipe(response, {
					end: true
				});

				this._sniffProxyResponse(request, proxyResponse);
			});

			// Pipe the data to the proxy
			request.pipe(proxyRequest, { end: true });
		});
		this.server.setTimeout(this.options.timeoutMs, (socket) => {
			Log.error("TIMEOUT server {} ({} ms)", this.target.hostname, this.server.timeout);
			console.log(socket);
		});

		this.server.keepAliveTimeout = this.options.timeoutMs;
		this.server.headersTimeout = this.options.timeoutMs;

		this.server.on("clientError", (e, socket) => {
			Log.error("{}: {:j}", this.target.hostname, e);
			socket.end("HTTP/1.1 400 Bad Request\r\n\r\n");
		});

		this.server.listen(this.port, () => {
			Log.info("Deployed Proxy to '{}' listening on port {}", this.target.href, this.port);
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
