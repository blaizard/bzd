import BodyParser from "body-parser";
import Compression from "compression";
import CookieParser from "cookie-parser";
import Express from "express";
import Minify from "express-minify";
import Helmet from "helmet";
import Http from "http";
import Https from "https";
import Multer from "multer";
import Path from "path";
import { WebSocketServer } from "ws";
import Router from "#bzd/nodejs/core/router.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";

import Event from "../event.mjs";
import { ExceptionFactory, ExceptionPrecondition } from "../exception.mjs";
import FileSystem from "../filesystem.mjs";
import LogFactory from "../log.mjs";
import { HttpServerContext, HttpError, WebsocketServerContext } from "#bzd/nodejs/core/http/server_context.mjs";
import HttpEndpoint from "#bzd/nodejs/core/http/endpoint.mjs";

const Log = LogFactory("http", "server");
const Exception = ExceptionFactory("http", "server");

export default class HttpServer {
	constructor(port, config) {
		Exception.assert(typeof port === "number", "The port '{}' must be a number.", port);
		this.config = Object.assign(
			{
				/// \brief Set the upload directory.
				uploadDir: false,
				/// \brief Set the file transfer limit.
				limit: 1 * 1024 * 1024,
				/// \brief Default request timeout.
				timeoutS: 2 * 60,
				/// \brief Public key of the SSL certificate.
				key: null,
				/// \brief SSL certificate.
				cert: null,
				/// \brief SSL certificate authority.
				ca: null,
				/// \brief Use data compression and minfy certain file types.
				useCompression: true,
			},
			config,
		);
		this.maxTimeoutS = this.config.timeoutS;
		this.routerWebsockets = new Router();
		this.statistics = new StatisticsProvider("http.server");

		this.event = new Event({
			ready: { proactive: true },
			error: { proactive: true },
		});

		this._initialize(port).catch((e) => {
			this.event.trigger("error", e);
			throw e;
		});
	}

	/**
	 * If any of the certificate file is set, assume SSL is to be used.
	 */
	_isSSL() {
		return Boolean(this.config.key || this.config.cert || this.config.ca);
	}

	/**
	 * Initialize the web module
	 */
	async _initialize(port) {
		this.app = Express();

		this.app.use(
			Helmet({
				// Only use Strict-Transport-Security if SSL is enabled
				hsts: this._isSSL(),
				// and Content-Security-Policy
				contentSecurityPolicy: this._isSSL(),
			}),
		);

		// Error handler
		this.app.use(middlewareErrorHandler);

		// Cookie parser
		this.app.use(CookieParser());

		// Enable compression
		if (this.config.useCompression) {
			this.app.use(Compression());
			this.app.use(
				Minify({
					cache: false, // use memory cache
				}),
			);
		}

		// Set the storage information for upload (if needed)
		{
			let diskStorageConfig = {
				filename: (req, file, cb) => {
					this._initialize.uid = this._initialize.uid || 0;
					cb(null, Date.now() + "-" + String(this._initialize.uid++) + "-" + file.originalname);
				},
			};
			if (this.config.uploadDir) {
				// Create the directory if it does not exists
				await FileSystem.mkdir(this.config.uploadDir);

				diskStorageConfig.destination = (req, file, cb) => {
					cb(null, this.config.uploadDir);
				};
			}
			this.storage = Multer.diskStorage(diskStorageConfig);
		}

		// Save the port number for connection
		this.port = port;

		this.event.trigger("ready");
	}

	/// Hook a websocket server to the http server.
	async startWebsocketServer(server) {
		this.wss = new WebSocketServer({ noServer: true });
		this.wss.on("connection", async (ws, request, options) => {
			const context = new WebsocketServerContext(ws, options.params);

			try {
				await options.handler(context);
			} catch (e) {
				if (e instanceof ExceptionPrecondition) {
					ws.close(1008, e.message);
					return;
				} else if (options.exceptionGuard) {
					Exception.print("Exception Guard; {}", Exception.fromError(e));
					ws.close(1008, e.message);
					return;
				} else {
					throw e;
				}
			}

			ws.isAlive = true;
			ws.on("pong", () => {
				ws.isAlive = true;
			});

			ws.on("error", (error) => {
				Exception.print("Receive error from websocket {}; {}", request.url, error);
			});
		});
		server.on("upgrade", (request, socket, head) => {
			const result = this.routerWebsockets.match(request.url);

			// Ignore if no result.
			if (!result) {
				socket.destroy();
				return;
			}

			const options = {
				handler: result.args,
				params: result.vars,
				/// Add guards to unhandled exceptions. This adds a callstack layer.
				exceptionGuard: true,
			};
			this.wss.handleUpgrade(request, socket, head, (ws) => {
				this.wss.emit("connection", ws, request, options);
			});
		});
		// Handle heartbeats for websockets to ensure inactive connections are closed.
		this.websocketsInterval = setInterval(() => {
			this.wss.clients.forEach((ws) => {
				if (ws.isAlive === false) {
					Log.info("Terminating dead websocket connection.");
					return ws.terminate();
				}

				// Otherwise, the connection is still alive.
				ws.isAlive = false;
				ws.ping();
			});
			this.statistics.set("websocket-clients", this.wss.clients.size);
		}, 30000); // 30 seconds
	}

	/// Stop the websocket server.
	async stopWebsocketServer() {
		await new Promise((resolve) => {
			this.wss.close(() => {
				resolve();
			});
		});

		clearInterval(this.websocketsInterval);

		this.wss.clients.forEach((ws) => {
			if (ws.readyState === ws.OPEN) {
				ws.close(1000, "Server shutting down");
			}
		});
	}

	/// Start the web server
	async start() {
		await this.event.waitUntil("ready");

		let configStrList = [];

		// Create the server
		if (this._isSSL()) {
			let options = {};
			if (this.config.key) {
				options.key = await FileSystem.readFile(this.config.key);
			}
			if (this.config.cert) {
				options.cert = await FileSystem.readFile(this.config.cert);
			}
			if (this.config.ca) {
				options.ca = await FileSystem.readFile(this.config.ca);
			}
			this.server = Https.createServer(options, this.app);
			configStrList.push("SSL");
		} else {
			this.server = Http.createServer(this.app);
		}

		if (this.config.useCompression) {
			configStrList.push("compression");
		}

		// This can only be set at server level.
		// Adds 1 min to make sure the proper timeout handler is triggered before the connection is closed.
		this.server.requestTimeout = (this.maxTimeoutS + 60) * 1000;

		/// Start the websocket server.
		await this.startWebsocketServer(this.server);

		return new Promise((resolve, reject) => {
			this.server.listen(this.port, undefined, undefined, () => {
				resetErrorHandler.call(this, reject);
				Log.info(
					"Web server serving at http://localhost:{}{}",
					this.port,
					configStrList.length ? " (" + configStrList.join(" and ") + ")" : "",
				);
				resolve();
			});
			this.server.on("error", (e) => {
				resetErrorHandler.call(this, reject);
				reject(new Exception(e));
			});
		});
	}

	/// Stop the web server
	async stop() {
		// Close the websocket servers.
		await this.stopWebsocketServer();

		return new Promise((resolve, reject) => {
			this.server.on("error", (e) => {
				resetErrorHandler.call(this, reject);
				reject(new Exception(e));
			});
			this.server.close(() => {
				resetErrorHandler.call(this, reject);
				resolve();
			});
		});
	}

	/**
	 * \brief Add a custom route to serve static files with the web server.
	 *
	 * \param uri The uri to which the request should match.
	 * \param path The path to which the static files will be served.
	 * \param options (optional) To be used by the static route
	 */
	async addStaticRoute(uri, path, options) {
		const absolutePath = Path.resolve(path);
		Exception.assert(absolutePath, "Absolute pat for path '{}' is empty.", path);
		Exception.assert(await FileSystem.exists(absolutePath), "No file are present at path '{}'.", absolutePath);

		options = Object.assign(
			{
				maxAge: 60 * 60 * 1000, // 1h
				index: "index.html",
				fallback: "index.html",
				headers: {},
			},
			options,
		);

		// Create the Epxress options
		const indexAbsolute = Path.join(absolutePath, options.index);
		const optionsExpress = {
			maxAge: options.maxAge,
			index: options.index,
			setHeaders: (res, path) => {
				// Disable caching of the index and during development.
				if (path == indexAbsolute || process.env.NODE_ENV == "development") {
					res.header("Cache-Control", "max-age=0, no-cache, no-store, must-revalidate");
					res.header("Pragma", "no-cache");
					res.header("Expires", "0");
				}
				Object.entries(options.headers).forEach(([key, value]) => {
					res.header(key, value);
				});
			},
		};
		this.app.use(uri, Express.static(absolutePath, optionsExpress));

		// If there is a fallback file
		if (options.fallback) {
			const fallback = Path.join(absolutePath, options.fallback);
			Exception.assert(await FileSystem.exists(fallback), "The fallback is not present at path '{}'.", fallback);
			this.app.use(uri, (req, res, next) => {
				if ((req.method === "GET" || req.method === "HEAD") && req.accepts("html")) {
					// dotfiles option is important here to allow serving files starting with a dot (.cache for example).
					res.status(200).sendFile(fallback, { dotfiles: "allow" }, (err) => {
						if (err) {
							console.error("Error sending fallback file:", fallback, err);
							res.status(500).send("<h1>Internal Server Error</h1><p>Could not load fallback content.</p>");
						}
					});
				} else {
					next();
				}
			});
		}
	}

	/// \brief Add a custom websocket route to the web server.
	///
	/// \param uri The endpoint to which the request should match.
	/// \param callback A callback that will be called if the uri and type matches
	///                 the request.
	addRouteWebsocket(uri, callback) {
		this.routerWebsockets.add(uri, null, callback);
	}

	/// \brief Add one or multiple custom route to the web server.
	///
	/// \param type The type of HTTP request (get, post, put, delete or patch).
	/// \param uri The endpoint to which the request should match.
	/// \param callback A callback that will be called if the uri and type matches
	///                 the request.
	/// \param options (optional) Extra options to be passed to the handler.
	addRoute(type, uri, callback, options) {
		// Update the options
		options = Object.assign(
			{
				/// Data type expected
				type: [],
				limit: this.config.limit,
				path: null,
				timeoutS: this.config.timeoutS,
				/// Add guards to unhandled exceptions. This adds a callstack layer.
				exceptionGuard: false,
			},
			options,
		);
		this.maxTimeoutS = Math.max(this.maxTimeoutS, options.timeoutS);

		const endpoint = new HttpEndpoint(uri);
		let callbackList = [middlewareErrorHandler];

		callbackList.unshift(async function (request, response) {
			// Create the context.
			const context = new HttpServerContext(request, response);
			// Override the params.
			// This is needed because Express apply a urldecode operation to the params which is not
			// wanted with variable arguments. Because we need to differentiate between / and %2F.
			const match = endpoint.match(request.path);
			Exception.assert(
				match,
				"There must be a match with the path ({}) and its regular expression ({}).",
				request.path,
				endpoint.toRegexp(),
			);
			request.params = match;
			try {
				await callback.call(this, context);
			} catch (e) {
				if (e instanceof HttpError) {
					e.send(context);
				} else if (e instanceof ExceptionPrecondition) {
					response.status(400).send(e.message);
				} else if (options.exceptionGuard) {
					Exception.print("Exception Guard; {}", Exception.fromError(e));
					response.status(500).send(e.message);
				} else {
					throw e;
				}
			}
		});

		if (options.type.indexOf("json") != -1) {
			callbackList.unshift(BodyParser.urlencoded({ limit: options.limit, extended: true }));
			callbackList.unshift(BodyParser.json({ limit: options.limit }));
		}

		if (options.type.indexOf("raw") != -1) {
			callbackList.unshift(BodyParser.raw({ limit: options.limit, type: () => true }));
		}

		if (options.type.indexOf("upload") != -1) {
			let upload = Multer({
				storage: this.storage,
				limits: { fileSize: options.limit },
			});
			callbackList.unshift(upload.any());
		}

		// Set specific options at the beginning to ensure they are processed before the following layer.
		callbackList.unshift((request, response, next) => {
			request.setTimeout(options.timeoutS * 1000);
			response.setTimeout(options.timeoutS * 1000);
			next();
		});
		switch (type) {
			case "*":
				this.app.all(endpoint.toRegexp(), ...callbackList);
				break;
			default:
				this.app[type.toLowerCase()](endpoint.toRegexp(), ...callbackList);
		}

		Log.debug("Added route: {} {} with options {:j}", type, uri, options);
	}
}

// ---- Private members ----

function resetErrorHandler(reject) {
	this.server.on("error", (e) => {
		reject(e);
	});
}

// It is important that the signature of this function contains the "next" argument,
// oterhwise it will not be handled correctly. From Express documentation:
// "error-handling functions have four arguments instead of three: (err, req, res, next)"
// eslint-disable-next-line no-unused-vars
function middlewareErrorHandler(e, req, res, next) {
	Exception.print("Receive error; {}", e);
	res.status(500).send(e.message);
}
