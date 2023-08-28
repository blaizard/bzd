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
				onAuthentication: (/*isAuthenticated*/) => {},
			},
			defaultOptions,
			options,
		);
	}

	installAPI(api) {
		return this._installAPIImpl(api);
	}

	/**
	 * Return true if the client is authenticated, false otherwise.
	 */
	isAuthenticated() {
		return this._isAuthenticatedImpl();
	}

	/**
	 * Attempt to refresh an authentication
	 */
	async refreshAuthentication() {
		return this._refreshAuthenticationImpl();
	}

	/**
	 * Attempt to login
	 */
	async login(api, uid, password, persistent = false) {
		return this._loginImpl(api, uid, password, persistent);
	}

	/**
	 * Attempt to logout
	 */
	async logout(api) {
		return this._logoutImpl(api);
	}

	/**
	 * Set authentication information to the fetch request
	 */
	async setAuthenticationFetch(fetchOptions) {
		return this._setAuthenticationFetchImpl(fetchOptions);
	}

	/**
	 * Make an authentication URL from a base URL
	 */
	async makeAuthenticationURL(url) {
		return this._makeAuthenticationURLImpl(url);
	}
}
