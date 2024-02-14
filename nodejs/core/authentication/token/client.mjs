import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import AuthenticationClient from "../client.mjs";
import Mutex from "#bzd/nodejs/core/mutex.mjs";
import Session from "../session.mjs";
import Validation from "../../validation.mjs";
import Cookie from "#bzd/nodejs/core/cookie.mjs";

const Exception = ExceptionFactory("authentication", "token");
const Log = LogFactory("authentication", "token");

export default class TokenAuthenticationClient extends AuthenticationClient {
	constructor(options) {
		super(options, {
			// Callback to refresh an invalid token.
			// Returns a dictionary with the token and refresh_token keys.
			// Returns null in case the refresh got invalidated.
			refreshTokenCallback: null,
		});

		// Token to be used during this session.
		this.token = undefined;
		this.session = undefined;
		// Used to set the state of the authentication.
		this.mutex = new Mutex();
		this.interval = null;

		this.validationResult = new Validation({
			uid: "mandatory",
			scopes: "mandatory",
			token: "mandatory",
			timeout: "mandatory",
		});
	}

	async _installAPIImpl(api) {
		Log.debug("Installing token-based authentication API.");

		if (!this.options.refreshTokenCallback) {
			this.options.refreshTokenCallback = async () => {
				const maybeRefreshToken = this.getRefreshToken();
				const data = maybeRefreshToken ? { refresh_token: maybeRefreshToken } : {};
				return await api.request("post", "/auth/refresh", data);
			};
			await this.tryRefreshAuthentication(/*nothrow*/ true);
		}
	}

	getRefreshToken() {
		return Cookie.get("local_refresh_token", null);
	}

	setRefreshToken(token, timeoutS = 0) {
		let options = {
			// Set the cookie valid for all path and sub-path of the website.
			path: "/",
		};
		if (timeoutS) {
			options.maxAge = timeoutS * 1000;
		}

		Cookie.set("local_refresh_token", token, options);
	}

	/// Get the current session if any.
	async _getSessionImpl() {
		if (this.session === undefined) {
			// Try to refresh the current token if any
			await this.tryRefreshAuthentication(/*nothrow*/ true);
		}

		return this.session ? this.session : null;
	}

	/// Set a new session from the result of the API.
	setSession(data) {
		this.validationResult.validate(data);

		this.token = data.token;
		if ("refresh_token" in data) {
			this.setRefreshToken(data.refresh_token, data.refresh_timeout);
		}

		this.session = new Session(data.uid, data.scopes);
		this.options.onAuthentication(this.session);

		if (this.interval) {
			clearInterval(this.interval);
		}
		if (data.timeout && this.options.refreshTokenCallback) {
			// Refresh automatically the token, set to minimum time of 30s
			this.interval = setInterval(
				() => {
					this._refreshAuthenticationImpl();
				},
				Math.max(data.timeout - 60, 30) * 1000,
			);
		}
	}

	/// Clear a session.
	clearSession() {
		this.token = null;
		this.session = null;
		Cookie.remove("local_refresh_token");
		this.options.onAuthentication(null);

		if (this.interval) {
			clearInterval(this.interval);
		}
	}

	/// Try to refresh the token
	async tryRefreshAuthentication(nothrow = false) {
		if (this.options.refreshTokenCallback) {
			try {
				await this.mutex.lock();
				const result = await this.options.refreshTokenCallback();
				if (result) {
					this.setSession(result);
					return result;
				}
			} catch (e) {
				this.clearSession();
				if (!nothrow && e.code != 401 /*Unauthorized*/) {
					throw e;
				}
			} finally {
				await this.mutex.release();
			}
		}
		return false;
	}

	async _refreshAuthenticationImpl() {
		// Try to refresh the token
		const result = await this.tryRefreshAuthentication(/*nothrow*/ false);
		if (result) {
			return result;
		}
		this.clearSession();
		await this.unauthorizedCallback_(/*needAuthentication*/ true);
	}

	async _setAuthenticationFetchImpl(fetchOptions) {
		// Automatically add authentication information to the request
		fetchOptions.authentication = {
			type: "bearer",
			token: this.token,
		};
	}

	async _makeAuthenticationURLImpl(url) {
		url += (url.includes("?") ? "&t=" : "?t=") + encodeURIComponent(this.token);
		return url;
	}

	async _loginImpl(api, uid, password, persistent, identifier) {
		const result = await this.loginWithAPI(api, "post", "/auth/login", {
			uid: uid,
			password: password,
			persistent: persistent,
			identifier: identifier,
		});
		return result;
	}

	async _loginWithSSOImpl(api, ssoToken) {
		Cookie.removeAll();
		this.setRefreshToken(ssoToken);
		await this._refreshAuthenticationImpl();
	}

	async _loginWithAPIImpl(result) {
		this.setSession(result);
	}

	async _logoutImpl(api) {
		try {
			await api.request("post", "/auth/logout");
		} finally {
			/// Clear in all cases the session. The API might fail on serverless use case for example.
			this.clearSession();
		}
	}
}
