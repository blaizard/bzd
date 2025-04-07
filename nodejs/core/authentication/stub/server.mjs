import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import AuthenticationServer from "../server.mjs";
import Session from "../session.mjs";

const Exception = ExceptionFactory("authentication", "stub");
const Log = LogFactory("authentication", "stub");

/// This is a stub always valid authentication server implementation.
export default class StubAuthenticationServer extends AuthenticationServer {
	constructor(options) {
		super(options, {
			/// UID used for the stub authenticated user.
			uid: "stub",
			/// List of scopes to be added when authenticated.
			scopes: [],
		});
		this.tokens = {};

		Log.info("Using stub authentication with scopes: {}.", this.options.scopes);
	}

	_installRestImpl(rest) {}

	async _loginWithUIDImpl(context, uid, identifier, persistent) {
		return {};
	}

	/// Verify that a request is authenticated.
	///
	/// This verifies the access token.
	/// \note We cannot refresh the access token as part of this process
	/// because it needs to live in a query (not cookie) because of of fetch requests.
	async _verifyImpl(context) {
		const maybeAccessToken = AuthenticationServer._getAccessToken(context);
		let scopes = this.options.scopes;

		// Update the scopes if the token is part of the registered tokens.
		if (maybeAccessToken && maybeAccessToken in this.tokens) {
			scopes = scopes.concat(this.tokens[maybeAccessToken]);
		}

		return new Session(this.options.uid, scopes);
	}

	async _makeSSOTokenImpl(identifier, session) {
		return "stub_token";
	}

	/// Preload an application token.
	///
	/// An application token is a token given to a third-party application to access resources.
	/// Unlike normal access tokens, it does not resolve to a UID, but does provide a scope.
	///
	/// \param token The token to be preloaded.
	/// \param scopes The scopes to be assigned to this token.
	async _preloadApplicationTokenImpl(token, scopes) {
		Exception.assert(!(token in this.tokens), "The token '{}' has been registered twice.", token);
		this.tokens[token] = scopes;
	}
}
