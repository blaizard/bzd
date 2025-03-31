import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("authentication");

export default class AuthenticationClient {
	constructor(options, defaultOptions = {}) {
		this.options = Object.assign(
			{
				// Callback that will be triggered if the user attempt to access
				// a route that requires authentication without it.
				unauthorizedCallback: null,
				// Callback to be called each time the authentication status changes.
				onAuthentication: (/*session or null*/) => {},
			},
			defaultOptions,
			options,
		);
	}

	installRest(rest) {
		this._installRestImpl(rest);
	}

	/// Return the session when authenticated, null otherwise.
	async isAuthenticated() {
		return Boolean(await this._getSessionImpl());
	}

	/// Internal helper to call unauthorizedCallback.
	async unauthorizedCallback_(needAuthentication) {
		if (this.options.unauthorizedCallback) {
			await this.options.unauthorizedCallback(needAuthentication);
		}
		if (needAuthentication) {
			Exception.unreachable("Unauthorized (need authentication)");
		} else {
			Exception.unreachable("Unauthorized");
		}
	}

	/// Ensure that there is either an authentication or at least one scope is associated with this session.
	async assertScopes(scopeOrScopesOrBoolean) {
		const maybeSession = await this._getSessionImpl();
		if (!maybeSession) {
			await this.unauthorizedCallback_(/*needAuthentication*/ true);
		} else if (typeof scopeOrScopesOrBoolean == "boolean") {
			return;
		} else if (!maybeSession.getScopes().matchAny(scopeOrScopesOrBoolean)) {
			await this.unauthorizedCallback_(/*needAuthentication*/ false);
		}
	}

	/// Return the session when authenticated, null otherwise.
	async getSession() {
		return await this._getSessionImpl();
	}

	/// Attempt to login
	async login(api, uid, password, persistent = false, identifier = "") {
		return await this._loginImpl(api, uid, password, persistent, identifier);
	}

	/// Attempt to login with a SSO token
	async loginWithSSO(api, ssoToken) {
		return await this._loginWithSSOImpl(api, ssoToken);
	}

	/// Attempt to login with internal data.
	async loginWithRest(api, method, path, data) {
		const result = await api.request(method, path, data);
		return await this._loginWithRestImpl(result);
	}

	/// Attempt to logout
	async logout(api) {
		Exception.assert(this.hasLogout(), "Logout is not available with this authentication schema.");
		await this._logoutImpl(api);
	}

	/// Set authentication information to the fetch request if authenticated.
	///
	/// \return true if success, false if not authenticated.
	async updateAuthenticationFetch(fetchOptions) {
		if (await this.isAuthenticated()) {
			await this._updateAuthenticationFetchImpl(fetchOptions);
			return true;
		}
		return false;
	}

	/// Make an authentication URL from a base URL if authenticated.
	///
	/// \return the URL updated or not.
	async updateAuthenticationURL(url) {
		if (await this.isAuthenticated()) {
			return await this._updateAuthenticationURLImpl(url);
		}
		return url;
	}

	/// Whether the logout functionality is available or not.
	hasLogout() {
		return typeof this._logoutImpl === "function";
	}
}
