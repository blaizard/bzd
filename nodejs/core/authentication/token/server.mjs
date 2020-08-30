import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import Validation from "../../validation.mjs";
import { makeUid } from "../../../utils/uid.mjs";
import AuthenticationServer from "../server.mjs";
import User from "../user.mjs";
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
			tokenRefreshLongTermExpiresIn: 7 * 24 * 60 * 60
		});

		this.validationRefreshToken = new Validation({
			uid: "mandatory",
			roles: "mandatory",
			persistent: "mandatory",
			session: "mandatory"
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

	_installAPIImpl(api) {
		Log.debug("Installing token-based authentication API.");
		api.addSchema(APISchema);

		const authentication = this;
		const generateTokens = async function(uid, roles, persistent, session) {
			// Generates the refresh token and set it to a cookie
			const refreshToken = await authentication.generateRefreshToken(uid, roles, persistent, session);

			/*
			 * Validate the authentication for this UID and session
			 * and return some arguments to be passed to the access token
			 */
			if (!(await authentication.verifyRefresh(uid, session, refreshToken.timeout))) {
				return this.setStatus(401, "Unauthorized");
			}

			this.setCookie("refresh_token", refreshToken.token, {
				httpOnly: true,
				maxAge: refreshToken.timeout * 1000
			});

			// Generate the access token
			return await authentication.generateAccessToken({ uid: uid, roles: roles });
		};

		api.handle("post", "/auth/login", async function(inputs) {
			// Verify uid/password pair
			const userInfo = await authentication.verifyIdentity(inputs.uid, inputs.password);
			if (userInfo) {
				return generateTokens.call(this, userInfo.uid, userInfo.roles, inputs.persistent, makeUid(userInfo.uid));
			}
			return this.setStatus(401, "Unauthorized");
		});

		api.handle("post", "/auth/logout", async function() {
			this.deleteCookie("refresh_token");
		});

		api.handle("post", "/auth/refresh", async function() {
			const refreshToken = this.getCookie("refresh_token", null);
			if (refreshToken == null) {
				return this.setStatus(401, "Unauthorized");
			}

			let data = null;
			try {
				data = await authentication.readToken(refreshToken);

				/*
				 * This ensures that a wrongly formatted token will trigger a new token being generated
				 * while still having the error reported. This ensure smooth transition to new token format.
				 */
				authentication.validationRefreshToken.validate(data);
			}
			catch (e) {
				Log.error("{}", e);
				return this.setStatus(401, "Unauthorized");
			}

			// Check access here
			return generateTokens.call(this, data.uid, data.roles, data.persistent || false, data.session);
		});
	}

	async _verifyImpl(request, callback) {
		let token = null;
		if ("authorization" in request.headers) {
			const data = request.headers["authorization"].split(" ");
			if (data.length == 2 && data[0].toLowerCase() == "bearer") {
				token = data[1];
			}
		}

		if ("t" in request.query) {
			token = request.query.t;
		}

		if (token) {
			return await this.verifyToken(token, callback);
		}

		return false;
	}

	async verifyToken(token, verifyCallback) {
		let data = null;
		try {
			data = await this.readToken(token);
			Exception.assert(data && "uid" in data, "Invalid token: {:j}", data);
			Exception.assert(data && "roles" in data, "Invalid token: {:j}", data);
		}
		catch (e) {
			return false;
		}
		return await verifyCallback(new User(data.uid, data.roles));
	}

	async generateAccessToken(data) {
		return await this._generateToken(data, this.options.tokenAccessExpiresIn);
	}

	async generateRefreshToken(uid, roles, persistent, session) {
		return await this._generateToken(
			{ uid: uid, roles: roles, persistent: persistent, session: session },
			persistent ? this.options.tokenRefreshLongTermExpiresIn : this.options.tokenRefreshShortTermExpiresIn
		);
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
