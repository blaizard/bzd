import ExceptionFactory from "../exception.mjs";
import Session from "./session.mjs";

const Exception = ExceptionFactory("authentication", "server");

export default class AuthenticationServer {
	/// Error type for verifyIdentity callback.
	/// Emulation of an enum type.
	static ErrorVerifyIdentity = Object.freeze({
		unauthorized: "unauthorized",
		tooManyAttempts: "tooManyAttempts",
	});

	constructor(options, defaultOptions = {}) {
		this.options = Object.assign(
			{
				/// Callback to verify once identity, matching uid and password pair if provided.
				/// This function can be called with only the uid, this case must be taken into account.
				/// In return on success, it will return the UID to be used and scopes as a dictionary.
				/// Otherwise, an error Result with a code from ErrorVerifyIdentity should be returned.
				/// The signature of this function is `async (uid, password)`.
				/// It should return an error Result if it doesn't verify. Otherwise, it should return a Result of
				/// a dict containing the `uid` and `scopes` as key.
				verifyIdentity: null,
				/// Refresh and verify a token. Returns true in case of success, false otherwise or
				/// a string corresponding to the new token in case of rolling tokens.
				refreshToken: null,
			},
			defaultOptions,
			options,
		);
	}

	installRest(rest) {
		this._installRestImpl(rest);
	}

	/// Attempt to login with a UID only.
	///
	/// \param context The http context.
	/// \param uid The uid to be used for login.
	/// \param identifier The identifier corresponding to this connection.
	/// \param persistent If the connection should be persistent or not.
	/// \return the same data as login.
	async loginWithUID(context, uid, identifier, persistent) {
		return await this._loginWithUIDImpl(context, uid, identifier, persistent);
	}

	/// Called when a request is received on the server side, to verify that the content is properly authorized.
	/// For example, check that the header contains a valid token.
	///
	/// \param context The http context.
	/// \param scopes Scopes to be in the session.
	///
	/// \return The session in case of success, false otherwise.
	async verify(context, scopes = null) {
		const maybeSession = await this._verifyImpl(context);
		if (!maybeSession) {
			return false;
		}
		if (scopes) {
			if (!maybeSession.getScopes().matchAny(scopes)) {
				return false;
			}
		}
		return maybeSession;
	}

	/// Create a single sign on token.
	///
	/// \param identifier The identifier name associated with this session.
	/// \param session The current session.
	/// \param scopes The scopes requested.
	async makeSSOToken(identifier, session, scopes) {
		Exception.assertPreconditionResult(session.getScopes().checkValid(scopes));
		const ssoSession = new Session(session.getUid(), scopes);
		return await this._makeSSOTokenImpl(identifier, ssoSession);
	}

	/// Preload an application token.
	///
	/// An application token is a token given to a third-party application to access resources.
	/// Unlike normal access tokens, it does not resolve to a UID, but does provide a scope.
	///
	/// \param token The token to be preloaded.
	/// \param scopes The scopes to be assigned to this token.
	async preloadApplicationToken(token, scopes = null) {
		return await this._preloadApplicationTokenImpl(token, scopes);
	}

	/// Get the access token from an HTTP context.
	///
	/// \param context The HTTP context.
	/// \return The access token if any, null otherwise.
	static _getAccessToken(context) {
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
}
