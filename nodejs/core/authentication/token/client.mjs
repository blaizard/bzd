import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import AuthenticationClient from "../client.mjs";
import APISchema from "./api.json";

const Exception = ExceptionFactory("authentication", "token");
const Log = LogFactory("authentication", "token");

export default class TokenAuthenticationClient extends AuthenticationClient {
	constructor(options) {
		super(options, {
			/**
			 * Callback to refresh an invalid token.
			 * Returns a dictionary with the token and refresh_token keys.
			 * Returns null in case the refresh got invalidated.
			 */
			refreshTokenCallback: null,
		});

		// Token to be used during this session.
		this.token = null;
		this.interval = null;
	}

	isAuthenticated() {
		return this.token !== null;
	}

	installAPI(api) {
		Log.debug("Installing token-based authentication API.");

		api.addSchema(APISchema);
		if (!this.options.refreshTokenCallback) {
			this.options.refreshTokenCallback = async () => {
				return await api.request("post", "/auth/refresh");
			};
		}

		// Try to refresh the current token if any
		this.tryRefreshAuthentication(/*nothrow*/ true);
	}

	setToken(token, timeoutS = 0) {
		const isPreviousAuthenticated = this.isAuthenticated();
		this.token = token;
		if (isPreviousAuthenticated != this.isAuthenticated()) {
			this.options.onAuthentication(this.isAuthenticated());
		}

		if (this.interval) {
			clearInterval(this.interval);
		}
		if (token && timeoutS && this.options.refreshTokenCallback) {
			// Refresh automatically the token, set to minimum time of 30s
			this.interval = setInterval(() => {
				this.refreshAuthentication();
			}, Math.max(timeoutS - 60, 30) * 1000);
		}
	}

	/**
	 * Try to refresh the token
	 */
	async tryRefreshAuthentication(nothrow = false) {
		if (this.options.refreshTokenCallback) {
			try {
				const result = await this.options.refreshTokenCallback();
				if (result) {
					Exception.assert("token" in result, "Missing token.");
					this.setToken(result.token, result.timeout);
					return true;
				}
			}
			catch (e) {
				if (!nothrow && e.code != 401 /*Unauthorized*/) {
					throw e;
				}
			}
		}
		return false;
	}

	async refreshAuthentication() {
		// Try to refresh the token
		if (await this.tryRefreshAuthentication(/*nothrow*/ false)) {
			return;
		}
		this.setToken(null);
		this.options.onAuthentication(false);

		if (this.options.unauthorizedCallback) {
			await this.options.unauthorizedCallback();
		}

		Exception.unreachable("Unauthorized");
	}

	async setAuthenticationFetch(fetchOptions) {
		if (!this.isAuthenticated()) {
			await this.refreshAuthentication();
		}

		// Automatically add authentication information to the request
		fetchOptions.authentication = {
			type: "bearer",
			token: this.token,
		};
	}

	async makeAuthenticationURL(url) {
		if (!this.isAuthenticated()) {
			await this.refreshAuthentication();
		}

		url += (url.includes("?") ? "&t=" : "?t=") + encodeURIComponent(this.token);
		return url;
	}

	async login(api, uid, password, persistent = false) {
		const result = await api.request("post", "/auth/login", {
			uid: uid,
			password: password,
			persistent: persistent,
		});
		this.setToken(result.token, result.timeout);
		return true;
	}

	async logout(api) {
		await api.request("post", "/auth/logout");
		this.setToken(null);
	}
}
