"use strict";

import Base from "./base.mjs";
import ExceptionFactory from "../exception.mjs";
import LogFactory from "../log.mjs";

const Exception = ExceptionFactory("api", "server");
const Log = LogFactory("api", "server");

class Context {

	constructor(request, response) {
		this.request = request;
		this.response = response;
		this.manualResponse = false;
	}

	setCookie(name, value, options) {
		options = Object.assign({
			maxAge: 7 * 24 * 60 * 60 * 1000, // in ms
			httpOnly: false
		});
		this.response.cookie(name, value, options);
	}

	getCookie(name, defaultValue) {
		return (name in this.request.cookies) ? this.request.cookies[name] : defaultValue;
	}

	setStatus(code, message = null) {
		this.response.status(code);
		if (message) {
			this.response.send(message);
		}
		else {
			this.response.end();
		}
		this.manualResponse = true;
	}
};

export default class APIServer extends Base {

    /**
     * Register a callback to handle a request
     */
	handle(web, method, endpoint, callback, /*options = {}*/) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		let authentication = null;
		if (this.schema[endpoint][method].authentication) {
			Exception.assert(this.options.authentication, "This route has authentication requirement but no authentication object was specified.");
			authentication = this.options.authentication;
		}

		// Build the web options
		let webOptions = {};
		if ("type" in requestOptions) {
			webOptions.type = [requestOptions.type];
		}

		// Create a wrapper to the callback
		const callbackWrapper = async function(request, response) {

			let context = new Context(request, response);

			try {

				// Check if this is a request that needs authentication
				if (authentication) {
					let authenticated = false;
					console.log(request.headers);
					if ("authorization" in request.headers) {
						const data = request.headers["authorization"].split(" ");
						if (data.length == 2 && data[0] == "token") {
							authenticated = authentication.verifyToken(data[1], (data) => {
								return true;
							});
						}
					}
					if (!authenticated) {
						return context.setStatus(401);
					}
				}

				let data = null;
				switch (requestOptions.type) {
				case "json":
					data = request.body.data;
					break;
				case "query":
					data = request.query;
					break;
				case "upload":
					data = Object.assign({}, request.query || {}, {
						files: Object.keys(request.files || {}).map((key) => request.files[key].path)
					});
					break;
				}

				const result = await callback.call(context, data);
				if (!context.manualResponse) {
					switch (responseOptions.type) {
					case "json":
						Exception.assert(typeof result == "object", "{} {}: callback result must be a json object.", method, endpoint);
						response.json(result);
						break;
					case "file":
						if (typeof result == "string") {
							response.sendFile(result);
						}
						else if ("pipe" in result) {
							await (new Promise((resolve, reject) => {
								result.on("error", reject)
									.on("end", resolve)
									.on("finish", resolve)
									.pipe(response);
							}));
							response.end();
						}
						else {
							Exception.unreachable("{} {}: callback result is not of a supported format.", method, endpoint);
						}
						break;
					case "raw":
						response.status(200).send(result);
						break;
					default:
						response.sendStatus(200);
					}
				}
			}
			catch (e) {
				Exception.print("Exception Guard");
				Exception.print(Exception.fromError(e));
				response.status(500).send(e.message);
			}
		};

		web.addRoute(method, this._makePath(endpoint), callbackWrapper, webOptions);
	}

	/**
	 * Setup endpoints for authentication
	 */
	handleAuthentication(web, endpointLogin, endpointRefresh) {

		Exception.assert(this.isAuthentication(), "Authentication is not supported on this server");
		const authentication = this.options.authentication;

		const generateTokens = async function (uid) {

			// Generates the refresh tocken and set it to a cookie
			const refreshToken = await authentication.generateRefreshToken({ uid: uid });
			this.setCookie("refresh_token", refreshToken.token, {
				httpOnly: true,
				maxAge: refreshToken.expiresIn * 1000
			});
	
			// Generate the access token
			return await authentication.generateAccessToken({ uid: uid });
		};

		this.handle(web, "post", endpointLogin, async function (inputs) {
			Exception.assert("uid" in inputs, "Missing uid: {:j}", inputs);
			Exception.assert("password" in inputs, "Missing password: {:j}", inputs);

			// Verify uid/password pair
			if (await authentication.verifyIdentity(inputs.uid, inputs.password)) {
				return generateTokens.call(this, inputs.uid);
			}
			return this.setStatus(403, "Forbidden");
		});
	
		this.handle(web, "post", endpointRefresh, async function () {

			const refreshToken = this.getCookie("refresh_token", null);

			let data = null;
			try {
				Exception.assert(refreshToken !== null, "No refresh token available");
				data = await authentication.readToken(refreshToken);
			}
			catch (e) {
				Log.error(e);
				return this.setStatus(403, "Forbidden");
			}

			// Check access here
			Exception.assert(data && "uid" in data, "Invalid token: {:j}", data);
			return generateTokens.call(this, data.uid);
		});
	}
}
