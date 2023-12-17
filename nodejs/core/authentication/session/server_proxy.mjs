import SessionAuthenticationServer from "#bzd/nodejs/core/authentication/session/server.mjs";
import APIClient from "#bzd/nodejs/core/api/client.mjs";
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

		this.apiRemote = new APIClient(APISchemaRemote, {
			host: this.options.remote,
			authentication: this,
		});
	}

	async _installAPIImpl(api) {
		Exception.assert(this.options.remote, "The remote server option 'remote' must be set.");

		Log.info("Installing session-based authentication API with proxy toward {}.", this.options.remote);

		const apiRemote = this.apiRemote;
		const authentication = this;
		api.handle("post", "/auth/refresh", async function (inputs) {
			/// Make sure the input is set and not implicitly coming from the cookie.
			if (!inputs.refresh_token) {
				const maybeRefreshToken = this.getCookie("refresh_token", null);
				if (maybeRefreshToken) {
					inputs.refresh_token = maybeRefreshToken;
				}
			}

			try {
				const result = await apiRemote.request("post", "/auth/refresh", inputs);

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

		api.handle("post", "/auth/logout", async function () {
			authentication.clearSession(this);
		});
	}
}
