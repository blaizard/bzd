export default class AuthenticationClient {
	constructor(options, defaultOptions = {}) {
		this.options = Object.assign(
			{
				/**
				 * Callback that will be triggered if the user attempt to access
				 * a route that requires authentication without it.
				 */
				unauthorizedCallback: null,
				/**
				 * Callback to be called each time the authentication status changes.
				 */
				onAuthentication: (/*session or null*/) => {},
			},
			defaultOptions,
			options,
		);
	}

	async installAPI(api) {
		await this._installAPIImpl(api);
	}

	/// Return the session when authenticated, null otherwise.
	async isAuthenticated() {
		return Boolean(await this._getSessionImpl());
	}

	/// Return the session when authenticated, null otherwise.
	async getSession() {
		return await this._getSessionImpl();
	}

	/// Attempt to refresh an authentication
	async refreshAuthentication() {
		return await this._refreshAuthenticationImpl();
	}

	/// Attempt to login
	async login(api, uid, password, persistent = false, identifier = "") {
		return await this._loginImpl(api, uid, password, persistent, identifier);
	}

	/// Attempt to logout
	async logout(api) {
		await this._logoutImpl(api);
	}

	/// Set authentication information to the fetch request
	async setAuthenticationFetch(fetchOptions) {
		return await this._setAuthenticationFetchImpl(fetchOptions);
	}

	/// Make an authentication URL from a base URL
	async makeAuthenticationURL(url) {
		return await this._makeAuthenticationURLImpl(url);
	}
}
