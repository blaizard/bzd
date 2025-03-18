import AuthenticationClient from "../client.mjs";
import Session from "../session.mjs";

export default class StubAuthenticationClient extends AuthenticationClient {
	constructor(options) {
		super(options, {
			/// UID used for the stub authenticated user.
			uid: "stub",
			/// List of scopes to be added when authenticated.
			scopes: [],
		});
	}

	_installRestImpl(rest) {
		this.options.onAuthentication(this._getSessionImpl());
	}

	/// Get the current session if any.
	async _getSessionImpl() {
		return new Session(this.options.uid, this.options.scopes);
	}

	async _updateAuthenticationFetchImpl(fetchOptions) {
		return true;
	}

	async _updateAuthenticationURLImpl(url) {
		return url;
	}

	async _loginImpl(rest, uid, password, persistent, identifier) {}

	async _loginWithSSOImpl(rest, ssoToken) {}

	async _loginWithRestImpl(result) {}

	async _logoutImpl(rest) {}
}
