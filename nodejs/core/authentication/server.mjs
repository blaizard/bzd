import ExceptionFactory from "../exception.mjs";
import Session from "./session.mjs";

const Exception = ExceptionFactory("authentication", "server");

export default class AuthenticationServer {
	constructor(options, defaultOptions = {}) {
		this.options = Object.assign(
			{
				/// Callback to verify once identiy, mathing uid and password pair.
				/// In return on success, it will return the UID to be used and roles as a dictionary.
				/// Otherwise, false should be returned.
				/// The signature of this function is `async (uid, password)`.
				/// It should return false if it doesn't verify. Otherwise, it should return a dict
				/// containing the `uid` and `roles` as key.
				verifyIdentity: null,
				/// Refresh and verify a token. Returns true in case of success, false otherwise or
				/// a string corresponding to the new token in case of rolling tokens.
				refreshToken: null,
			},
			defaultOptions,
			options,
		);
	}

	async installAPI(api) {
		await this._installAPIImpl(api);
	}

	/// Called when a request is received on the server side, to verify that the content is properly authorized.
	/// For example, check that the header contains a valid token.
	///
	/// \param context The http context.
	/// \param callback the function to be called when the verification succeed, it should be populated with the user.
	async verify(context, callback = async (/*user*/) => true) {
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
