import SessionAuthenticationServerProxy from "#bzd/nodejs/core/authentication/session/server_proxy.mjs";

export default class AccountsAuthenticationServer extends SessionAuthenticationServerProxy {
	constructor(options) {
		Object.assign(
			{
				/// Address of the remote server to use with this proxy.
				accounts: null,
			},
			options,
		);
		super({
			remote: options.accounts,
		});
	}
}
