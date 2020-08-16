import Express from "express";
import Helmet from "helmet";
import Path from "path";
import BodyParser from "body-parser";
import Http from "http";
import Https from "https";
import Compression from "compression";
import Minify from "express-minify";
import Multer from "multer";
import CookieParser from "cookie-parser";

import Event from "./event.mjs";
import FileSystem from "./filesystem.mjs";
import LogFactory from "./log.mjs";
import ExceptionFactory from "./exception.mjs";

const Log = LogFactory("web");
const Exception = ExceptionFactory("web");

export default class Web {
	constructor(port, config) {
		this.config = Object.assign(
			{
				/**
				 * \brief Set the upload directory.
				 */
				uploadDir: false,
				/**
				 * \brief Set the file transfer limit.
				 */
				limit: 1 * 1024 * 1024,
				/**
				 * \brief Public key of the SSL certificate.
				 */
				key: null,
				/**
				 * \brief SSL certificate.
				 */
				cert: null,
				/**
				 * \brief SSL certificate authority.
				 */
				ca: null,
				/**
				 * \brief Use data compression and minfy certain file types.
				 */
				useCompression: true,
				/**
				 * Options to be used while serving static files
				 */
				staticOptions: {
					maxAge: 60 * 60 * 1000, // 1h
					index: "index.html",
					setHeaders: (res, path) => {
						/*
						 * Do not cache the index.html
						 * this is usefull when the application updates.
						 */
						if (path.endsWith("index.html")) {
							res.header("Cache-Control", "private, no-cache, no-store, must-revalidate");
							res.header("Expires", "-1");
							res.header("Pragma", "no-cache");
						}
					}
				}
			},
			config
		);

		this.event = new Event({
			ready: { proactive: true },
			error: { proactive: true }
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
				hsts: this._isSSL()
			})
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
					cache: false // use memory cache
				})
			);
		}

		// Set the storage information for upload (if needed)
		{
			let diskStorageConfig = {
				filename: (req, file, cb) => {
					this._initialize.uid = this._initialize.uid || 0;
					cb(null, Date.now() + "-" + String(this._initialize.uid++) + "-" + file.originalname);
				}
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
		}
		else {
			this.server = Http.createServer(this.app);
		}

		if (this.config.useCompression) {
			configStrList.push("compression");
		}

		return new Promise((resolve, reject) => {
			this.server.listen(this.port, undefined, undefined, () => {
				resetErrorHandler.call(this, reject);
				Log.info(
					"Web server serving at http://localhost:{}{}",
					this.port,
					configStrList.length ? " (" + configStrList.join(" and ") + ")" : ""
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
	 * \param fallback (optional) Serve a specific file if nothing else can be served.
	 */
	async addStaticRoute(uri, path, fallback) {
		const absolutePath = Path.resolve(path);
		Exception.assert(absolutePath, "Absolute pat for path '{}' is empty.", path);
		Exception.assert(await FileSystem.exists(absolutePath), "No file are present at path '{}'.", absolutePath);

		this.app.use(uri, Express.static(absolutePath, this.config.staticOptions));

		// If there is a fallback file
		if (fallback) {
			this.app.use(
				uri,
				((...args) => (req, res, next) => {
					if ((req.method === "GET" || req.method === "HEAD") && req.accepts("html")) {
						(res.sendFile || res.sendfile).call(res, ...args, (err) => err && next());
					}
					else {
						next();
					}
				})(fallback, { root: absolutePath })
			);
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
				/**
				 * Add guards to unhandled exceptions. This adds a callstack layer.
				 */
				exceptionGuard: false
			},
			options
		);

		let callbackList = [middlewareErrorHandler];

		if (options.exceptionGuard) {
			callbackList.unshift(async function(request, response) {
				try {
					await callback.call(this, request, response);
				}
				catch (e) {
					Exception.print("Exception Guard; {}", Exception.fromError(e));
					response.status(500).send(e.message);
				}
			});
		}
		else {
			callbackList.unshift(callback);
		}

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
				limits: { fileSize: options.limit }
			});
			callbackList.unshift(upload.any());
		}

		switch (type.toLowerCase()) {
		case "get":
			this.app.get(uri, ...callbackList);
			break;
		case "post":
			this.app.post(uri, ...callbackList);
			break;
		case "put":
			this.app.put(uri, ...callbackList);
			break;
		case "delete":
			this.app.delete(uri, ...callbackList);
			break;
		case "patch":
			this.app.patch(uri, ...callbackList);
			break;
		default:
			throw new Exception("Unknown HTTP type '{}'.", type);
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

/*
 * It is important that the signature of this function contains the "next" argument,
 * oterhwise it will not be handled correctly. From Express documentation:
 * "error-handling functions have four arguments instead of three: (err, req, res, next)"
 */
// eslint-disable-next-line no-unused-vars
function middlewareErrorHandler(e, req, res, next) {
	Exception.print("Receive error; {}", e);
	res.status(500).send(e.message);
}
