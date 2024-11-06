import Crypto from "crypto";

import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import AuthenticationServer from "../server.mjs";
import Session from "../session.mjs";
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
			//     sessions: {
			//     	    <hash>: {scopes: [<scope>, ...], expiration: <timestamp>},
			//	   		<hash>: {scopes: [<scope>, ...], expiration: <timestamp>},
			//     }
			// }
			// Ordered from the earliest first.
			kvs: null,
			// The bucket name for the sessions to be stored in the key value store.
			kvsBucket: "sessions",
			// Function to save a refresh token somewhere.
			// Its signature is as follow: async (session, hash, durationS, identifier, rolling) { ... }
			saveRefreshToken: null,
			// Function to remove an existing refresh token previously saved.
			// Its signature is as follow: async (uid, hash) { ... }
			removeRefreshToken: null,
			// Maximum session allowed per user.
			maxSessionsPerUser: 5,
			// Time in seconds after which the access token will expire.
			tokenAccessExpiresIn: 5 * 60,
			// Reuse token that have x seconds left.
			tokenAccessExpiresInReuse: 60,
			// Time in seconds after which the SSO refresh token will expire.
			tokenRefreshSSOExpiresIn: 2 * 60,
			// Time in seconds after which the refresh token will expire.
			tokenRefreshShortTermExpiresIn: 30 * 60,
			// Time in seconds after which the refresh token will expire.
			tokenRefreshLongTermExpiresIn: 7 * 24 * 60 * 60,
		});

		if (this.options.kvs === null) {
			this.options.kvs = new KeyValueStoreMemory("session-authentication");
		}
	}

	_installRestImpl(rest) {
		Exception.assert(
			this.options.saveRefreshToken,
			"The callback to save a refresh token must be set, set 'saveRefreshToken'.",
		);
		Exception.assert(
			this.options.removeRefreshToken,
			"The callback to remove a refresh token must be set, set 'removeRefreshToken'.",
		);

		Log.info("Installing session-based authentication REST.");

		const authentication = this;

		// This endpoint returns a refresh token.
		// A refresh token is stored in the user database and is similar to a OAuth token.
		// It can be exchanged to an access token with /auth/refresh.
		rest.handle("post", "/auth/login", async function (inputs) {
			// Verify uid/password pair
			Exception.assertPrecondition(inputs.password, "Missing password");
			return await authentication._login(this, inputs.identifier, inputs.persistent, inputs.uid, inputs.password);
		});

		rest.handle("post", "/auth/logout", async function () {
			authentication.clearSession(this);
		});

		/// Use a refresh token to create an access token.
		///
		/// If refresh_token is given as an argument, return the tokens by values, if not, return it from the cookies.
		rest.handle("post", "/auth/refresh", async function (inputs) {
			let maybeTokenObject = false;

			// Check if there is an access token and that it is still valid.
			const maybeAccessToken = authentication._getAccessToken(this);
			if (maybeAccessToken) {
				const result = await authentication._verifyAccessToken(
					maybeAccessToken,
					authentication.options.tokenAccessExpiresInReuse,
				);
				if (result) {
					maybeTokenObject = {
						token: authentication._makeToken(result.session.getUid(), result.hash),
						timeout: result.expiration - authentication._getTimestamp(),
						uid: result.session.getUid(),
						scopes: result.session.getScopes().toList(),
					};
				}
			}

			// If not, refresh the token from the refresh_token.
			if (!maybeTokenObject) {
				const maybeRefreshToken =
					"refresh_token" in inputs ? inputs.refresh_token : this.getCookie("refresh_token", null);
				maybeTokenObject = await authentication._getAndRefreshAccessToken(this, maybeRefreshToken);
			}

			if (!maybeTokenObject) {
				throw this.httpError(401, "Unauthorized");
			}

			// If refresh token is coming from the cookie, set cookies as well.
			if (!("refresh_token" in inputs)) {
				// Set the access token.
				this.setCookie("access_token", maybeTokenObject.token, {
					httpOnly: true,
					sameSite: "strict",
					maxAge: maybeTokenObject.timeout * 1000,
				});

				// Set the refresh token (if needed).
				if ("refresh_token" in maybeTokenObject) {
					this.setCookie("refresh_token", maybeTokenObject.refresh_token, {
						httpOnly: true,
						sameSite: "strict",
						newMaxAge: maybeTokenObject.refresh_timeout ? maybeTokenObject.refresh_timeout * 1000 : undefined,
					});
					delete maybeTokenObject.refresh_token;
					delete maybeTokenObject.refresh_timeout;
				}
			}

			return maybeTokenObject;
		});

		// Verify a token, this is useful with the server proxy.
		rest.handle("post", "/auth/verify", async function (inputs) {
			const result = await authentication._verifyAccessToken(inputs.token);
			if (!result) {
				throw this.httpError(401, "Unauthorized");
			}
			return {
				uid: result.session.getUid(),
				scopes: result.scopes,
				hash: result.hash,
				timeout: result.expiration - authentication._getTimestamp(),
			};
		});
	}

	async clearSession(context) {
		// Remove the cookies
		const maybeAccessToken = this._getAccessToken(context);
		context.deleteCookie("access_token");
		const maybeRefreshToken = context.getCookie("refresh_token", null);
		context.deleteCookie("refresh_token");
		context.deleteCookie("local_refresh_token");

		if (maybeAccessToken) {
			const [uid, hash] = this._readToken(maybeAccessToken);
			await this.options.kvs.update(this.options.kvsBucket, uid, (data) => {
				if (!data.sessions) {
					return { sessions: {} };
				}
				if (hash in data.sessions) {
					delete data.sessions[hash];
				}
				return data;
			});
		}

		if (maybeRefreshToken && this.options.removeRefreshToken) {
			const [uid, hash] = this._readToken(maybeRefreshToken);
			await this.options.removeRefreshToken(uid, hash);
		}
	}

	async _loginWithUIDImpl(context, uid, identifier, persistent) {
		return await this._login(context, identifier, persistent, uid);
	}

	async _login(context, identifier, persistent, uid, password = undefined) {
		const sessionInfoResult = await this.options.verifyIdentity(uid, password);
		if (sessionInfoResult.hasValue()) {
			const session = new Session(sessionInfoResult.value().uid, sessionInfoResult.value().scopes);

			const timeoutS = persistent
				? this.options.tokenRefreshLongTermExpiresIn
				: this.options.tokenRefreshShortTermExpiresIn;
			const refreshToken = await this._makeRefreshToken(identifier, session, timeoutS);

			context.setCookie("refresh_token", refreshToken, {
				httpOnly: true,
				sameSite: "strict",
				maxAge: timeoutS * 1000,
			});

			// Generate the access token.
			return await this._makeAccessToken(session);
		}

		switch (sessionInfoResult.error()) {
			case AuthenticationServer.ErrorVerifyIdentity.tooManyAttempts:
				throw context.httpError(429, "Too Many Requests");
			case AuthenticationServer.ErrorVerifyIdentity.unauthorized:
			default:
				throw context.httpError(401, "Unauthorized");
		}
	}

	async _makeSSOTokenImpl(identifier, session) {
		return await this._makeRefreshToken(identifier, session, this.options.tokenRefreshSSOExpiresIn);
	}

	async _makeRefreshToken(identifier, session, timeoutS) {
		const hash = this._makeTokenHash();
		await this.options.saveRefreshToken(session, hash, timeoutS, identifier, /*rolling*/ true);
		return this._makeToken(session.getUid(), hash);
	}

	/// Get the current timestamp in seconds.
	_getTimestamp() {
		return Math.floor(Date.now() / 1000);
	}

	/// Create a new session, save it and return the token.
	async _makeAccessToken(session) {
		// Check if there is an existing access token with a valid expiration date that can be re-used.
		const maybeSessions = await this.options.kvs.get(this.options.kvsBucket, session.getUid(), null);
		if (maybeSessions) {
			for (const [hash, sessionData] of Object.entries(maybeSessions.sessions)) {
				const timeoutS = sessionData.expiration - this._getTimestamp();
				// If too old, do not consider anymore, as this array is ordered.
				if (timeoutS >= this.options.tokenAccessExpiresInReuse) {
					// If the session has the same scopes, reuse it1
					if (session.getScopes().sameAs(sessionData.scopes)) {
						return {
							token: this._makeToken(session.getUid(), hash),
							timeout: timeoutS,
							uid: session.getUid(),
							scopes: session.getScopes().toList(),
						};
					}
				}
			}
		}

		const hash = this._makeTokenHash();
		const sessionData = {
			expiration: this._getTimestamp() + this.options.tokenAccessExpiresIn,
			scopes: session.getScopes().toList(),
		};

		// Insert the new session and return the token.
		await this.options.kvs.update(
			this.options.kvsBucket,
			session.getUid(),
			(data) => {
				// Remove the sessions that will expire the first if too many.
				// This operation should not be frequent, hence the sorting is not expensive.
				if (Object.keys(data.sessions).length >= this.options.maxSessionsPerUser) {
					const sortedSessions = Object.entries(data.sessions).sort((a, b) => b.expiration - a.expiration);
					data.sessions = Object.fromEntries(sortedSessions.slice(0, this.options.maxSessionsPerUser - 1));
				}
				// Add the new session.
				data.sessions[hash] = sessionData;
				return data;
			},
			{ sessions: {} },
		);

		// Return the token
		return {
			token: this._makeToken(session.getUid(), hash),
			timeout: this.options.tokenAccessExpiresIn,
			uid: session.getUid(),
			scopes: session.getScopes().toList(),
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
	///
	/// This verifies the access token.
	/// \note We cannot refresh the access token as part of this process
	/// because it needs to live in a query (not cookie) because of of fetch requests.
	async _verifyImpl(context) {
		const maybeToken = this._getAccessToken(context);
		if (!maybeToken) {
			return false;
		}
		const result = await this._verifyAccessToken(maybeToken);
		if (!result) {
			return false;
		}
		return result.session;
	}

	/// Refresh an access token from a refresh token.
	async _getAndRefreshAccessToken(context, maybeRefreshToken = null) {
		if (maybeRefreshToken == null) {
			return false;
		}

		const [uid, hash] = this._readToken(maybeRefreshToken);
		const maybeToken = await this.options.refreshToken(uid, hash, () => {
			return {
				hash: this._makeTokenHash(),
				minDuration: this.options.tokenAccessExpiresIn,
			};
		});
		if (!maybeToken) {
			return false;
		}

		// Create the access token.
		const session = new Session(maybeToken.uid, maybeToken.scopes);
		let maybeAccessToken = await this._makeAccessToken(session);

		// If the token has a new hash or timeout, update the cookie.
		if (maybeAccessToken && ("hash" in maybeToken || "timeout" in maybeToken)) {
			Object.assign(maybeAccessToken, {
				refresh_token: maybeToken.hash ? this._makeToken(uid, maybeToken.hash) : maybeRefreshToken,
				refresh_timeout: maybeToken.timeout,
			});
		}

		return maybeAccessToken;
	}

	/// Get the access token from an HTTP context.
	///
	/// \param context The HTTP context.
	/// \return The access token if any, null otherwise.
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
			token = context.getCookie("access_token", null);
		}
		if (!token) {
			return null;
		}
		return token;
	}

	/// Check the status of an access token.
	///
	/// \param token The token to be checked.
	/// \param minTimeoutS Minimum timeout that make this token valid.
	/// \return A object with a "user" field and a "session" field for the token, false otherwise.
	async _verifyAccessToken(token, minTimeoutS = 0) {
		const [uid, hash] = this._readToken(token);
		const maybeSessions = await this.options.kvs.get(this.options.kvsBucket, uid, null);
		// No session for this user.
		if (maybeSessions === null || !("sessions" in maybeSessions)) {
			return false;
		}

		// Look for the token
		const maybeSession = maybeSessions.sessions[hash];
		if (!maybeSession) {
			return false;
		}

		// Expired.
		if (maybeSession.expiration < this._getTimestamp() + minTimeoutS) {
			return false;
		}

		return {
			session: new Session(uid, maybeSession.scopes),
			hash: hash,
			expiration: maybeSession.expiration,
			scopes: maybeSession.scopes,
		};
	}
}
