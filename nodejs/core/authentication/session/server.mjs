import Crypto from "crypto";

import { makeUid } from "../../../utils/uid.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import Validation from "../../validation.mjs";
import AuthenticationServer from "../server.mjs";
import User from "../user.mjs";

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
			//	      {hash: <hash>, roles: [<role>, ...], expiration: <timestamp>},
			//	      {hash: <hash>, roles: [<role>, ...], expiration: <timestamp>},
			//     ],
			//     uid: <uid>,
			// }
			kvs: null,
			// The bucket name for the sessions to be stored in the key value store.
			kvsBucket: "sessions",
			// Maximum session allowed per user.
			maxSessionsPerUser: 5,
			// Time in seconds after which the access token will expire.
			tokenAccessExpiresIn: 5 * 60,
			// Time in seconds after which the refresh token will expire.
			tokenRefreshShortTermExpiresIn: 30 * 60,
			// Time in seconds after which the refresh token will expire.
			tokenRefreshLongTermExpiresIn: 7 * 24 * 60 * 60,
		});

		Exception.assert(this.options.kvs, "A key-value-store must be set for session authentication, set 'kvs'.");

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
			const userInfo = await authentication.verifyIdentity(inputs.uid, inputs.password);
			if (userInfo) {
				return await this._createRefreshToken(userInfo);
			}
			return this.sendStatus(401, "Unauthorized");
		});

		api.handle("post", "/auth/logout", async function () {
			this.deleteCookie("refresh_token");
		});

		// Use a refresh token to create an access token.
		api.handle("post", "/auth/refresh", async function () {
			const refreshToken = this.getCookie("refresh_token", null);
			if (refreshToken == null) {
				return this.sendStatus(401, "Unauthorized");
			}

			let data = null;
			try {
				data = await authentication.readToken(refreshToken);

				// This ensures that a wrongly formatted token will trigger a new token being generated
				// while still having the error reported. This ensure smooth transition to new token format.
				authentication.validationRefreshToken.validate(data);
			} catch (e) {
				Exception.fromError(e).print();
				return this.sendStatus(401, "Unauthorized");
			}

			// Check access here
			return generateTokens.call(this, data.uid, data.roles, data.persistent || false, data.session);
		});
	}

	_getTimestamp() {
		return Math.floor(Date.now() / 1000);
	}

	/// Create a refresh token.
	async _createRefreshToken(user) {}

	/// Create a new session, save it and return the token.
	async _createSession(uid, roles) {
		const session = {
			hash: this._makeTokenHash(),
			expiration: this._getTimestamp() + this.options.tokenAccessExpiresIn,
			roles: roles,
		};

		// Insert the new session and return the token.
		const uidHash = this._makeUidHash(userInfo.uid);
		await this.options.kvs.update(
			this.options.kvsBucket,
			uidHash,
			(data) => {
				// Remove the number of sessions if too many.
				while (data.sessions.length >= this.options.maxSessionsPerUser) {
					data.shift();
				}
				data.push(session);
				return data;
			},
			{
				sessions: [],
				uid: uid,
			},
		);

		// Return the token
		return this._makeToken(uidHash, session.hash);
	}

	_makeUidHash(uid) {
		return Crypto.createHash("shake256", { outputLength: 16 }).update(uid).digest("hex");
	}

	_makeTokenHash() {
		return Crypto.createHash("shake256", { outputLength: 64 }).update(Math.random()).digest("hex");
	}

	_makeToken(uidHash, tokenHash) {
		return uidHash + tokenHash;
	}

	/// Read the data embedded inside a token. This data contains a UID + a hash.
	_readToken(token) {}

	async _verifyImpl(context, callback) {
		let token = null;
		const data = context.getHeader("authorization", "").split(" ");
		if (data.length == 2 && data[0].toLowerCase() == "bearer") {
			token = data[1];
		} else {
			token = context.getQuery("t");
		}

		if (token) {
			return await this._verifyToken(token, callback);
		}

		return false;
	}

	async _verifyToken(token, verifyCallback) {
		const maybeSession = await this.options.kvs.get(this.options.kvsBucket, token, null);
		// No session.
		if (maybeSession === null) {
			return false;
		}
		// Expired.
		if (maybeSession.expiration < Date.now()) {
			return false;
		}
		return await verifyCallback(new User(maybeSession.uid, maybeSession.roles));
	}
}
