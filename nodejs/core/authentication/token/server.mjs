

import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import AuthenticationServer from "../server.mjs";
import Jwt from "jsonwebtoken";
import APISchema from "./api.json";

const Exception = ExceptionFactory("authentication", "token");
const Log = LogFactory("authentication", "token");

export default class TokenAuthenticationServer extends AuthenticationServer {
	constructor(options) {
		super(options, {
			privateKey: null,
			publicKey: null,
			/**
			 * Time in seconds after which the access token will expire.
			 */
			tokenAccessExpiresIn: 15 * 60,
			/**
			 * Time in seconds after which the refresh token will expire.
			 */
			tokenRefreshShortTermExpiresIn: 30 * 60,
			/**
			 * Time in seconds after which the refresh token will expire.
			 */
			tokenRefreshLongTermExpiresIn: 7 * 24 * 60 * 60,
		});
	}

	async _generateToken(data, expiresIn) {
		Exception.assert(this.options.privateKey, "The private key is not set.");
		return new Promise((resolve, reject) => {
			Jwt.sign(data, this.options.privateKey, { expiresIn: expiresIn }, (e, token) => {
				if (e) {
					reject(e);
				}
				else {
					resolve({
						token: token,
						timeout: expiresIn
					});
				}
			});
		});
	}

	installAPI(api) {

		Log.debug("Installing token-based authentication API.");
		api.addSchema(APISchema);

		const authentication = this;
		const generateTokens = async function (uid, persistent) {

			// Generates the refresh tocken and set it to a cookie
			const refreshToken = await authentication.generateRefreshToken(uid, persistent);
			this.setCookie("refresh_token", refreshToken.token, {
				httpOnly: true,
				maxAge: refreshToken.timeout * 1000
			});
	
			// Generate the access token
			return await authentication.generateAccessToken({ uid: uid });
		};

		api.handle("post", "/auth/login", async function (inputs) {
			// Verify uid/password pair
			if (await authentication.verifyIdentity(inputs.uid, inputs.password)) {
				return generateTokens.call(this, inputs.uid, inputs.persistent);
			}
			return this.setStatus(401, "Unauthorized");
		});

		api.handle("post", "/auth/logout", async function () {
			this.deleteCookie("refresh_token");
		});

		api.handle("post", "/auth/refresh", async function () {

			const refreshToken = this.getCookie("refresh_token", null);
			if (refreshToken == null) {
				return this.setStatus(401, "Unauthorized");
			}

			let data = null;
			try {
				data = await authentication.readToken(refreshToken);
			}
			catch (e) {
				Log.error(e);
				return this.setStatus(401, "Unauthorized");
			}

			// Check access here
			Exception.assert(data && "uid" in data, "Invalid token: {:j}", data);
			return generateTokens.call(this, data.uid, data.persistent || false);
		});
	}

	async verify(request, callback = (/*uid*/) => true) {
		let token = null;
		if ("authorization" in request.headers) {
			const data = request.headers["authorization"].split(" ");
			if (data.length == 2 && data[0] == "bearer") {
				token = data[1];
			}
		}

		if ("t" in  request.query) {
			token = request.query.t;
		}

		if (token) {
			return await this.verifyToken(token, callback);
		}

		return false;
	}

	async verifyToken(token, verifyCallback = (/*uid*/) => true) {
		try {
			const data = await this.readToken(token);
			return await verifyCallback(data.uid);
		}
		catch (e) {
			return false;
		}
	}

	async generateAccessToken(data) {
		return await this._generateToken(data, this.options.tokenAccessExpiresIn);
	}

	async generateRefreshToken(uid, persistent) {
		return await this._generateToken({uid: uid, persistent: persistent}, (persistent) ? this.options.tokenRefreshLongTermExpiresIn : this.options.tokenRefreshShortTermExpiresIn);
	}

	async readToken(token) {
		Exception.assert(this.options.publicKey || this.options.privateKey, "A key must be set.");
		return new Promise((resolve, reject) => {
			Jwt.verify(token, this.options.publicKey || this.options.privateKey, (e, data) => {
				if (e) {
					reject(e);
				}
				else {
					resolve(data);
				}
			});
		});
	}
}
