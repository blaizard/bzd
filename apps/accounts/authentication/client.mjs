import TokenAuthenticationClient from "#bzd/nodejs/core/authentication/token/client.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import StubAuthenticationClient from "#bzd/nodejs/core/authentication/stub/client.mjs";

const Exception = ExceptionFactory("authentication", "accounts");

export default class AccountsAuthenticationClient extends TokenAuthenticationClient {
	constructor(options) {
		super(options, {
			// URL of the accounts.
			remote: null,
		});

		Exception.assert(this.options.remote, "No remote URL was specified.");
	}

	static make(options) {
		const { type: type, ...optionsRest } = options;
		console.log(options, optionsRest, type);
		switch (type || "accounts") {
			case "accounts":
				return new AccountsAuthenticationClient(optionsRest);
			case "stub":
				return new StubAuthenticationClient(optionsRest);
			default:
				Exception.unreachable("Unsupported account type: '{}'.", type);
		}
	}

	_installRestImpl(rest) {
		// Register the hook to login
		rest.provide("login", (application) => {
			window.location.href = this.options.remote + "/login?application=" + application;
		});

		super._installRestImpl(rest);

		// Check if login from a SSO token can be done.
		const url = new URL(window.location.href);
		const maybeSSOToken = url.searchParams.get("sso_token");
		if (maybeSSOToken) {
			url.searchParams.delete("sso_token");
			window.history.replaceState({}, null, url.href);
			rest.loginWithSSO(maybeSSOToken).catch((e) => Exception.error(e));
		}
	}
}
