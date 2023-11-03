import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import AuthenticationClient from "../client.mjs";
import Mutex from "#bzd/nodejs/core/mutex.mjs";

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
		// Used to set the state of the authentication.
		this.mutex = new Mutex();
		this.interval = null;
	}

	async _isAuthenticatedImpl() {
		await this.mutex.lock();
		if (this.token === undefined) {
			// Try to refresh the current token if any
			await this.tryRefreshAuthentication(/*nothrow*/ true);
		}
		await this.mutex.release();

		return Boolean(this.token);
	}

	async _installAPIImpl(api) {
		Log.debug("Installing token-based authentication API.");

		if (!this.options.refreshTokenCallback) {
			this.options.refreshTokenCallback = async () => {
				return await api.request("post", "/auth/refresh");
			};
		}
	}

	setToken(token, timeoutS = 0) {
		this.token = token;
		this.options.onAuthentication(this.token !== null);

		if (this.interval) {
			clearInterval(this.interval);
		}
		if (token && timeoutS && this.options.refreshTokenCallback) {
			// Refresh automatically the token, set to minimum time of 30s
			this.interval = setInterval(
				() => {
					this.refreshAuthentication();
				},
				Math.max(timeoutS - 60, 30) * 1000,
			);
		}
	}

	/// Try to refresh the token
	async tryRefreshAuthentication(nothrow = false) {
		if (this.options.refreshTokenCallback) {
			try {
				const result = await this.options.refreshTokenCallback();
				if (result) {
					Exception.assert("token" in result, "Missing token.");
					Exception.assert("timeout" in result, "Missing timeout.");
					this.setToken(result.token, result.timeout);
					return true;
				}
			} catch (e) {
				this.setToken(null);
				if (!nothrow && e.code != 401 /*Unauthorized*/) {
					throw e;
				}
			}
		}
		return false;
	}

	async _refreshAuthenticationImpl() {
		// Try to refresh the token
		if (await this.tryRefreshAuthentication(/*nothrow*/ false)) {
			return;
		}
		this.setToken(null);

		if (this.options.unauthorizedCallback) {
			await this.options.unauthorizedCallback();
		}

		Exception.unreachable("Unauthorized");
	}

	async _setAuthenticationFetchImpl(fetchOptions) {
		if (!this.isAuthenticated()) {
			await this.refreshAuthentication();
		}

		// Automatically add authentication information to the request
		fetchOptions.authentication = {
			type: "bearer",
			token: this.token,
		};
	}

	async _makeAuthenticationURLImpl(url) {
		if (!this.isAuthenticated()) {
			await this.refreshAuthentication();
		}

		url += (url.includes("?") ? "&t=" : "?t=") + encodeURIComponent(this.token);
		return url;
	}

	async _loginImpl(api, uid, password, persistent) {
		const result = await api.request("post", "/auth/login", {
			uid: uid,
			password: password,
			persistent: persistent,
		});
		Exception.assert("token" in result, "Missing token.");
		Exception.assert("timeout" in result, "Missing timeout.");
		this.setToken(result.token, result.timeout);
	}

	async _logoutImpl(api) {
		await api.request("post", "/auth/logout");
		this.setToken(null);
	}
}
