"use strict";

const Log = require("./log.js")("web");
const Exception = require("./exception.js")("web");

const Express = require("express");
const Helmet = require("helmet");
const Path = require("path");
const BodyParser = require("body-parser");
const Http = require("http");
const Https = require("https");
const Fs = require("fs");
const Compression = require("compression");
const Minify = require("express-minify");
const Multer = require("multer");

module.exports = class Web {
	constructor(port, config) {
		this.config = Object.assign({
			/**
			 * \brief Root directory of the static data to provide.
			 */
			rootDir: false,
			/**
			 * \brief Set the upload directory.
			 */
			uploadDir: false,
			/**
			 * \brief Set the file transfer limit.
			 */
			limit: 1 * 1024 *1024,
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
				extensions: ["html", "htm"]
			}
		}, config);

		this.app = Express();
		this.app.use(Helmet());
		// Enable compression
		if (this.config.useCompression) {
			this.app.use(Compression());
			this.app.use(Minify({
				cache: true
			}));
		}

		// Set the storage information for upload (if needed)
		{
			let diskStorageConfig = {
				filename: (req, file, cb) => {
					cb(null, Date.now() + "-" + file.originalname);
				}
			};
			if (this.config.uploadDir) {
				// Create the directory if it does not exists
				if (!Fs.existsSync(this.config.uploadDir)){
					Fs.mkdirSync(this.config.uploadDir);
				}
				diskStorageConfig.destination = (req, file, cb) => {
					cb(null, this.config.uploadDir);
				};
			}
			this.storage = Multer.diskStorage(diskStorageConfig);
		}

		// Error handler
		this.app.use(middlewareErrorHandler);

		// Save the port number for connection
		this.port = port;

		// Web content
		if (this.config.rootDir) {
			this.addStaticRoute(this.config.rootDir);
			this.addRoute("get", "/", (req, res) => {
				res.sendFile(Path.resolve(this.config.rootDir, "index.html"));
			});
		}
	}

	/**
	 * \brief Type of data
	 * \{
	 */
	static get NORMAL() { return 0x00; }
	static get JSON() { return 0x01; }
	static get RAW() { return 0x02; }
	static get UPLOAD() { return 0x04; }
	/**
	 * \}
	 */

	/**
	 * Start the web server
	 */
	start() {
		return new Promise((resolve, reject) => {

			// If any of the certificate file is set, assume SSL is to be used
			const useSSL = this.config.key || this.config.cert || this.config.ca;
			let configStrList = [];

			// Create the server
			if (useSSL) {
				let options = {};
				if (this.config.key) options.key = Fs.readFileSync(this.config.key);
				if (this.config.cert) options.cert = Fs.readFileSync(this.config.cert);
				if (this.config.ca) options.ca = Fs.readFileSync(this.config.ca);
				this.server = Https.createServer(options, this.app);
				configStrList.push("SSL");
			}
			else {
				this.server = Http.createServer(this.app);
			}

			if (this.config.useCompression) {
				configStrList.push("compression");
			}

			this.server.listen(this.port, undefined, undefined, () => {
				resetErrorHandler.call(this, reject);
				Log.info("Web server started on port " + this.port
						+ ((configStrList.length) ? (" (" + configStrList.join(" and ") + ")") : ""));
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
	 * \param uri (optional) The uri to which the request should match.
	 * \param path The path to which the static files will be served.
	 */
	addStaticRoute(uri, path) {
		// Make uri optional
		if (typeof path === "undefined") {
			path = uri;
			uri = undefined;
		}

		const absolutePath = Path.resolve(path);
		if (!absolutePath || !Fs.existsSync(absolutePath)) {
			throw new Exception("The root directory '" + path + "' -> '" + absolutePath + "' does not exist.");
		}
		if (uri) {
			this.app.use(uri, Express.static(absolutePath, this.config.staticOptions));
		}
		else {
			this.app.use(Express.static(absolutePath, this.config.staticOptions));
		}
	}

	/**
	 * \brief Add a custom route to the web server.
	 *
	 * \param type The type of HTTP request (get, post, put, delete or patch).
	 * \param uri The uri to which the request should match.
	 * \param callback A callback that will be called if the uri and type matches
	 *                 the request.
	 * \param dataType (optional) The type of data to be recieved, can be Web.JSON,
	 *                 Web.RAW or Web.UPLOAD.
	 * \param options (optional) Extra options to be passed to the handler.
	 */
	addRoute(type, uri, callback, dataType, options) {

		if (typeof dataType === "undefined") {
			dataType = Web.NORMAL;
		}

		// Update the options
		options = Object.assign({
			limit: this.config.limit,
			path: null,
			/**
			 * Add guards to unhandled exceptions. This adds a callstack layer.
			 */
			exceptionGuard: false
		}, options);

		let callbackList = [middlewareErrorHandler];

		if (options.exceptionGuard) {
			callbackList.unshift(async function(request, response) {
				try {
					await callback.call(this, request, response);
				}
				catch (e) {
					Exception.print("Exception Guard", Exception.fromError(e));
					response.status(500).send(e.message);
				}
			});
		}
		else {
			callbackList.unshift(callback);
		}

		if (dataType & Web.JSON) {
			callbackList.unshift(BodyParser.urlencoded({ limit: options.limit, extended: true }));
			callbackList.unshift(BodyParser.json({ limit: options.limit }));
		}

		if (dataType & Web.RAW) {
			callbackList.unshift(BodyParser.raw({ limit: options.limit, type: () => true }));
		}

		if (dataType & Web.UPLOAD) {
			let upload = Multer({ storage: this.storage, limits: { fileSize: options.limit } });
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
			throw new Exception("Unknown HTTP type '" + type + "'.");
		}
	}
};

// ---- Private members ----

function resetErrorHandler(reject)
{
	this.server.on("error", (e) => {
		reject(e);
	});
}

// It is important that the signature of this function contains the "next" argument,
// oterhwise it will not be handled correctly. From Express documentation:
// "error-handling functions have four arguments instead of three: (err, req, res, next)"
// eslint-disable-next-line no-unused-vars
function middlewareErrorHandler(e, req, res, next)
{
	Exception.print("Receive error.", e);
	res.status(500).send(e.message);
}
