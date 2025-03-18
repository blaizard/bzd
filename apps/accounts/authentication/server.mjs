import SessionAuthenticationServerProxy from "#bzd/nodejs/core/authentication/session/server_proxy.mjs";
import StubAuthenticationServer from "#bzd/nodejs/core/authentication/stub/server.mjs";

export default class AccountsAuthenticationServer extends SessionAuthenticationServerProxy {
	constructor(options) {
		Object.assign(
			{
				/// Address of the remote server to use with this proxy.
				remote: null,
			},
			options,
		);
		super({
			remote: options.remote,
		});
	}

	static make(options) {
		const { type: type, ...optionsRest } = options;
		switch (type || "accounts") {
			case "accounts":
				return new AccountsAuthenticationServer(optionsRest);
			case "stub":
				return new StubAuthenticationServer(optionsRest);
			default:
				Exception.unreachable("Unsupported account type: '{}'.", type);
		}
	}
}
