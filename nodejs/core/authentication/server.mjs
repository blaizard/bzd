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
	/// \param callback the function to be called when the verification succeed, it should be populated with the session.
	async verify(context, callback = async (/*session*/) => true) {
		return this._verifyImpl(context, callback);
	}

	/// Create a single sign on token.
	///
	/// \param identifier The identifier name associated with this session.
	/// \param session The current session.
	/// \param scopes The scopes requested.
	async makeSSOToken(identifier, session, scopes) {
		Exception.assertPreconditionResult(session.getScopes().checkValid(scopes));
		const ssoSession = new Session(session.getUid(), scopes);
		return this._makeSSOTokenImpl(identifier, ssoSession);
	}
}
