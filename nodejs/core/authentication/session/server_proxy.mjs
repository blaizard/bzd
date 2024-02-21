import SessionAuthenticationServer from "#bzd/nodejs/core/authentication/session/server.mjs";
import RestClient from "#bzd/nodejs/core/rest/client.mjs";
import APISchemaRemote from "#bzd/nodejs/core/authentication/session/api.json" assert { type: "json" };
import { HttpClientException } from "#bzd/nodejs/core/http/client.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";

const Exception = ExceptionFactory("authentication", "session-proxy");
const Log = LogFactory("authentication", "session-proxy");

export default class SessionAuthenticationServerProxy extends SessionAuthenticationServer {
	constructor(options) {
		super(
			Object.assign(
				{
					/// Address of the remote server to use with this proxy.
					remote: null,
				},
				options,
			),
		);

		this.restRemote = new RestClient(APISchemaRemote, {
			host: this.options.remote,
			authentication: this,
		});
	}

	async _installRestImpl(rest) {
		Exception.assert(this.options.remote, "The remote server option 'remote' must be set.");

		Log.info("Installing session-based authentication REST with proxy toward {}.", this.options.remote);

		const restRemote = this.restRemote;
		const authentication = this;
		rest.handle("post", "/auth/refresh", async function (inputs) {
			/// Make sure the input is set and not implicitly coming from the cookie.
			if (!inputs.refresh_token) {
				const maybeRefreshToken = this.getCookie("refresh_token", null);
				if (maybeRefreshToken) {
					inputs.refresh_token = maybeRefreshToken;
				}
			}

			try {
				const result = await restRemote.request("post", "/auth/refresh", inputs);

				/// Make sure the local cookie is not used again.
				this.deleteCookie("local_refresh_token");
				this.setCookie("refresh_token", result.refresh_token, {
					httpOnly: true,
					sameSite: "strict",
					newMaxAge: result.refresh_timeout ? result.refresh_timeout * 1000 : undefined,
				});

				delete result.refresh_token;
				delete result.refresh_timeout;
				return result;
			} catch (e) {
				// Propagate the error to the caller.
				if (e instanceof HttpClientException) {
					throw this.httpError(e.code, e.message);
				}
				throw e;
			}
		});

		rest.handle("post", "/auth/logout", async function () {
			authentication.clearSession(this);
		});
	}
}
