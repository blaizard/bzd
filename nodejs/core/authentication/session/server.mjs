import Crypto from "crypto";

import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import Validation from "../../validation.mjs";
import AuthenticationServer from "../server.mjs";
import User from "../user.mjs";
import KeyValueStoreMemory from "#bzd/nodejs/db/key_value_store/memory.mjs";

const Exception = ExceptionFactory("authentication", "session");
const Log = LogFactory("authentication", "session");

/// The session authentication stores session information on the server side.
///
/// It contains 2 types of tokens.
/// - refresh token: This is the first one that is acquired after login. It is a longer
///                  term token (than the access) that is stored on the database along side
///                  the user data. User can revoke the access at any time. It can be used
///                  by a 3rd party to access user specific info.
///                  This token does not changed when refreshed, only its expiration date changes.
///                  The sole purpose of the refresh token is to convert it into an access token.
/// -  access token: This is the one used to authorized request to access specific information.
///                  It is short-lived and resides in a cache (or any fast access memory). It has
///                  a short expiry date.
///                  When refreshed it changes together with its expiry date.
export default class SessionAuthenticationServer extends AuthenticationServer {
	constructor(options) {
		super(options, {
			// Key value store to save the session IDs.
			// The structure of this KVS looks like this:
			// <uid-hash>: {
			//     sessions: [
			//     	    {hash: <hash>, roles: [<role>, ...], expiration: <timestamp>},
			//	   		{hash: <hash>, roles: [<role>, ...], expiration: <timestamp>},
			//     ]
			// }
			// Ordered from the earliest first.
			kvs: null,
			// The bucket name for the sessions to be stored in the key value store.
			kvsBucket: "sessions",
			// Function to save a refresh token somewhere.
			// Its signature is as follow: async (uid, hash, durationS, rolling) { ... }
			saveRefreshToken: null,
			// Function to remove an existing refresh token previously saved.
			// Its signature is as follow: async (uid, hash) { ... }
			removeRefreshToken: null,
			// Maximum session allowed per user.
			maxSessionsPerUser: 5,
			// Time in seconds after which the access token will expire.
			tokenAccessExpiresIn: 5 * 60,
			// Time in seconds after which the refresh token will expire.
			tokenRefreshShortTermExpiresIn: 30 * 60,
			// Time in seconds after which the refresh token will expire.
			tokenRefreshLongTermExpiresIn: 7 * 24 * 60 * 60,
		});

		if (this.options.kvs === null) {
			this.options.kvs = new KeyValueStoreMemory("session-authentication");
		}
		Exception.assert(
			this.options.saveRefreshToken,
			"The callback to save a refresh token must be set, set 'saveRefreshToken'.",
		);
		Exception.assert(
			this.options.removeRefreshToken,
			"The callback to remove a refresh token must be set, set 'removeRefreshToken'.",
		);

		this.validationRefreshToken = new Validation({
			uid: "mandatory",
			roles: "mandatory",
			persistent: "mandatory",
			session: "mandatory",
		});
	}

	_installAPIImpl(api) {
		Log.debug("Installing session-based authentication API.");

		const authentication = this;

		// This endpoint returns a refresh token.
		// A refresh token is stored in the user database and is similar to a OAuth token.
		// It can be exchanged to an access token with /auth/refresh.
		api.handle("post", "/auth/login", async function (inputs) {
			// Verify uid/password pair
			const userInfo = await authentication.options.verifyIdentity(inputs.uid, inputs.password);
			if (userInfo) {
				// Generate the refresh token.
				const hash = authentication._makeTokenHash();
				const timeoutS = inputs.persistent
					? authentication.options.tokenRefreshLongTermExpiresIn
					: authentication.options.tokenRefreshShortTermExpiresIn;
				await authentication.options.saveRefreshToken(userInfo.uid, hash, timeoutS, /*rolling*/ true);
				const refreshToken = authentication._makeToken(userInfo.uid, hash);
				this.setCookie("refresh_token", refreshToken, {
					httpOnly: true,
					maxAge: timeoutS * 1000,
				});

				// Generate the access token.
				return await authentication._makeAccessToken(userInfo);
			}

			throw this.httpError(401, "Unauthorized");
		});

		api.handle("post", "/auth/logout", async function () {
			const refreshToken = this.getCookie("refresh_token", null);
			if (refreshToken == null) {
				return;
			}

			const [uid, hash] = authentication._readToken(refreshToken);
			await authentication.options.removeRefreshToken(uid, hash);
			this.deleteCookie("refresh_token");
		});

		// Use a refresh token to create an access token.
		api.handle("post", "/auth/refresh", async function () {
			const maybeTokenObject = await authentication._getAndRefreshAccessToken(this);
			if (!maybeTokenObject) {
				throw this.httpError(401, "Unauthorized");
			}
			return maybeTokenObject;
		});
	}

	_getTimestamp() {
		return Math.floor(Date.now() / 1000);
	}

	/// Create a new session, save it and return the token.
	async _makeAccessToken(user) {
		const session = {
			hash: this._makeTokenHash(),
			expiration: this._getTimestamp() + this.options.tokenAccessExpiresIn,
			roles: user.roles,
		};

		// Insert the new session and return the token.
		await this.options.kvs.update(
			this.options.kvsBucket,
			user.uid,
			(data) => {
				// Remove the sessions that will expire the first if too many.
				if (data.length >= this.options.maxSessionsPerUser) {
					data.sessions = data.sessions.slice(0, this.options.maxSessionsPerUser - 1);
				}
				// Add the new session.
				data.sessions.unshift(session);
				return data;
			},
			{ sessions: [] },
		);

		// Return the token
		return {
			token: this._makeToken(user.uid, session.hash),
			timeout: this.options.tokenAccessExpiresIn,
		};
	}

	_makeTokenHash() {
		return Crypto.createHash("shake256", { outputLength: 32 }).update(Math.random().toString()).digest("hex");
	}

	_makeToken(uid, tokenHash) {
		return uid + "_" + tokenHash;
	}

	/// Read the data embedded inside a token. This data contains a UID + a hash.
	_readToken(token) {
		return token.split("_");
	}

	/// Verify that a request is authenticated.
	async _verifyImpl(context, callback) {
		let maybeUser = await this._getAndVerifyAccessToken(context);
		if (!maybeUser) {
			const maybeTokenObject = await this._getAndRefreshAccessToken(context);
			if (!maybeTokenObject) {
				return false;
			}
			maybeUser = await this._verifyAccessToken(maybeTokenObject.token);
			if (!maybeUser) {
				return false;
			}
		}
		return await callback(maybeUser);
	}

	/// Refresh an access token from a refresh token.
	async _getAndRefreshAccessToken(context) {
		const refreshToken = context.getCookie("refresh_token", null);
		if (refreshToken == null) {
			return false;
		}

		const [uid, hash] = this._readToken(refreshToken);
		const maybeToken = await this.options.refreshToken(uid, hash);
		if (!maybeToken) {
			return false;
		}

		// TODO: handle rolling token.

		// Create the access token.
		const userInfo = new User(maybeToken.uid, maybeToken.roles);
		const accessToken = await this._makeAccessToken(userInfo);
		context.setCookie("access_token", accessToken.token, {
			httpOnly: true,
			maxAge: accessToken.timeout * 1000,
		});

		return accessToken;
	}

	async _getAndVerifyAccessToken(context) {
		const maybeToken = this._getAccessToken(context);
		if (!maybeToken) {
			return false;
		}
		const maybeUser = await this._verifyAccessToken(maybeToken);
		if (!maybeUser) {
			return false;
		}
		return maybeUser;
	}

	_getAccessToken(context) {
		let token = null;
		const data = context.getHeader("authorization", "").split(" ");
		if (data.length == 2 && data[0].toLowerCase() == "bearer") {
			token = data[1];
		}
		if (!token) {
			token = context.getQuery("t");
		}
		if (!token) {
			token = this.getCookie("access_token", null);
		}
		if (!token) {
			return null;
		}
		return token;
	}

	async _verifyAccessToken(token) {
		const [uid, hash] = this._readToken(token);
		const maybeSessions = await this.options.kvs.get(this.options.kvsBucket, uid, null);
		// No session for this user.
		if (maybeSessions === null || !("sessions" in maybeSessions)) {
			return false;
		}

		// Look for the token
		const maybeSession = maybeSessions.sessions.find((session) => session.hash == hash);
		if (!maybeSession) {
			return false;
		}

		// Expired.
		if (maybeSession.expiration < this._getTimestamp()) {
			return false;
		}

		return new User(uid, maybeSession.roles);
	}
}
