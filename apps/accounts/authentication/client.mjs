import TokenAuthenticationClient from "#bzd/nodejs/core/authentication/token/client.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("authentication", "accounts");

export default class AccountsAuthenticationClient extends TokenAuthenticationClient {
	constructor(options) {
		super(options, {
			// URL of the accounts.
			accounts: null,
		});

		Exception.assert(this.options.accounts, "No accounts URL was specified.");
	}

	async _installRestImpl(rest) {
		// Register the hook to login
		rest.provide("login", (application) => {
			window.location.href = this.options.accounts + "/login?application=" + application;
		});

		await super._installRestImpl(rest);

		// Check if login from a SSO token can be done.
		const url = new URL(window.location.href);
		const maybeSSOToken = url.searchParams.get("sso_token");
		if (maybeSSOToken) {
			url.searchParams.delete("sso_token");
			window.history.replaceState({}, null, url.href);
			await rest.loginWithSSO(maybeSSOToken);
		}
	}
}
