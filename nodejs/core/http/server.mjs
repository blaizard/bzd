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

import Event from "../event.mjs";
import { ExceptionFactory, ExceptionPrecondition } from "../exception.mjs";
import FileSystem from "../filesystem.mjs";
import LogFactory from "../log.mjs";
import { HttpServerContext, HttpError } from "#bzd/nodejs/core/http/server_context.mjs";
import Endpoint from "#bzd/nodejs/core/http/endpoint.mjs";

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

	/**
	 * Start the web server
	 */
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

	/**
	 * Stop the web server
	 */
	stop() {
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
					optionsExpress.setHeaders(res, fallback);
					res.sendFile.call(res, fallback, {}, (err) => err && next());
				} else {
					next();
				}
			});
		}
	}

	/**
	 * \brief Add a custom route to the web server.
	 *
	 * \param type The type of HTTP request (get, post, put, delete or patch).
	 * \param uri The uri to which the request should match.
	 * \param callback A callback that will be called if the uri and type matches
	 *                 the request.
	 * \param options (optional) Extra options to be passed to the handler.
	 */
	addRoute(type, uri, callback, options) {
		// Update the options
		options = Object.assign(
			{
				/**
				 * Data type expected
				 */
				type: [],
				limit: this.config.limit,
				path: null,
				timeoutS: this.config.timeoutS,
				/**
				 * Add guards to unhandled exceptions. This adds a callstack layer.
				 */
				exceptionGuard: false,
			},
			options,
		);
		this.maxTimeoutS = Math.max(this.maxTimeoutS, options.timeoutS);

		const endpoint = new Endpoint(uri);
		const maybeRegexprPath = endpoint.isVarArgs() ? endpoint.toRegexp() : null;

		let callbackList = [middlewareErrorHandler];

		callbackList.unshift(async function (request, response) {
			// Create the context.
			const context = new HttpServerContext(request, response);
			// Override the params if there is a regexpr.
			// This is needed because Express apply a urldecode operation to the params which is not
			// wanted with variable arguments. Because we need to differentiate between / and %2F.
			if (maybeRegexprPath) {
				const match = maybeRegexprPath.exec(request.path);
				Exception.assert(
					match,
					"There must be a match with the path ({}) and its regular expression ({}).",
					request.path,
					maybeRegexprPath,
				);
				request.params = match.groups;
			}
			try {
				await callback.call(this, context);
			} catch (e) {
				if (e instanceof HttpError) {
					response.status(e.code).send(e.message);
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

		const updatedURI =
			"/" +
			endpoint
				.map((fragment) => {
					if (typeof fragment == "string") {
						return fragment;
					}
					if (fragment.isVarArgs) {
						return ":" + fragment.name + "(*)";
					}
					return ":" + fragment.name;
				})
				.join("/");

		switch (type) {
			case "*":
				this.app.all(updatedURI, ...callbackList);
				break;
			default:
				this.app[type.toLowerCase()](updatedURI, ...callbackList);
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
