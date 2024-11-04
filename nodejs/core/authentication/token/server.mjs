import Jwt from "jsonwebtoken";

import { makeUid } from "../../../utils/uid.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import Validation from "../../validation.mjs";
import AuthenticationServer from "../server.mjs";
import Session from "../session.mjs";

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

		this.validationRefreshToken = new Validation({
			uid: "mandatory",
			scopes: "mandatory",
			persistent: "mandatory",
			session: "mandatory",
		});
	}

	async _generateToken(data, expiresIn) {
		Exception.assert(this.options.privateKey, "The private key is not set.");
		return new Promise((resolve, reject) => {
			Jwt.sign(data, this.options.privateKey, { expiresIn: expiresIn }, (e, token) => {
				if (e) {
					reject(e);
				} else {
					resolve({
						token: token,
						timeout: expiresIn,
						uid: data.uid,
						scopes: data.scopes,
					});
				}
			});
		});
	}

	_installRestImpl(rest) {
		Log.debug("Installing token-based authentication REST.");

		const authentication = this;
		const generateTokens = async function (uid, scopes, persistent, session) {
			// Generates the refresh token and set it to a cookie
			const refreshToken = await authentication.generateRefreshToken(uid, scopes, persistent, session);

			// Validate the authentication for this UID and session
			// and return some arguments to be passed to the access token
			if (!(await authentication.options.refreshToken(uid, session, refreshToken.timeout))) {
				throw this.httpError(401, "Unauthorized");
			}

			this.setCookie("refresh_token", refreshToken.token, {
				httpOnly: true,
				maxAge: refreshToken.timeout * 1000,
			});

			// Generate the access token
			return await authentication.generateAccessToken({ uid: uid, scopes: scopes });
		};

		rest.handle("post", "/auth/login", async function (inputs) {
			// Verify uid/password pair
			const sessionInfoResult = await authentication.options.verifyIdentity(inputs.uid, inputs.password);
			if (sessionInfoResult.hasValue()) {
				return generateTokens.call(
					this,
					sessionInfoResult.value().uid,
					sessionInfoResult.value().scopes,
					inputs.persistent,
					makeUid(sessionInfoResult.value().uid),
				);
			}
			switch (sessionInfoResult.error()) {
				case AuthenticationServer.ErrorVerifyIdentity.tooManyAttempts:
					throw context.httpError(429, "Too Many Requests");
				case AuthenticationServer.ErrorVerifyIdentity.unauthorized:
				default:
					throw context.httpError(401, "Unauthorized");
			}
		});

		rest.handle("post", "/auth/logout", async function () {
			this.deleteCookie("refresh_token");
		});

		rest.handle("post", "/auth/refresh", async function () {
			const refreshToken = this.getCookie("refresh_token", null);
			if (refreshToken == null) {
				throw this.httpError(401, "Unauthorized");
			}

			let data = null;
			try {
				data = await authentication.readToken(refreshToken);

				// This ensures that a wrongly formatted token will trigger a new token being generated
				// while still having the error reported. This ensure smooth transition to new token format.
				authentication.validationRefreshToken.validate(data);
			} catch (e) {
				Exception.fromError(e).print();
				throw this.httpError(401, "Unauthorized");
			}

			// Check access here
			return generateTokens.call(this, data.uid, data.scopes, data.persistent || false, data.session);
		});
	}

	async _verifyImpl(context) {
		let token = null;
		const data = context.getHeader("authorization", "").split(" ");
		if (data.length == 2 && data[0].toLowerCase() == "bearer") {
			token = data[1];
		} else {
			token = context.getQuery("t");
		}

		if (token) {
			return await this.verifyToken(token);
		}

		return false;
	}

	async verifyToken(token) {
		let data = null;
		try {
			data = await this.readToken(token);
			Exception.assert(data && "uid" in data, "Invalid token: {:j}", data);
			Exception.assert(data && "scopes" in data, "Invalid token: {:j}", data);
		} catch (e) {
			return false;
		}
		return new Session(data.uid, data.scopes);
	}

	async generateAccessToken(data) {
		return await this._generateToken(data, this.options.tokenAccessExpiresIn);
	}

	async generateRefreshToken(uid, scopes, persistent, session) {
		return await this._generateToken(
			{ uid: uid, scopes: scopes, persistent: persistent, session: session },
			persistent ? this.options.tokenRefreshLongTermExpiresIn : this.options.tokenRefreshShortTermExpiresIn,
		);
	}

	async readToken(token) {
		Exception.assert(this.options.publicKey || this.options.privateKey, "A key must be set.");
		return new Promise((resolve, reject) => {
			Jwt.verify(token, this.options.publicKey || this.options.privateKey, (e, data) => {
				if (e) {
					reject(e);
				} else {
					resolve(data);
				}
			});
		});
	}
}
