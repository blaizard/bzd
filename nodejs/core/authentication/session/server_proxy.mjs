import SessionAuthenticationServer from "#bzd/nodejs/core/authentication/session/server.mjs";
import RestClient from "#bzd/nodejs/core/rest/client.mjs";
import APISchemaRemote from "#bzd/nodejs/core/authentication/session/api.json" with { type: "json" };
import { HttpClientException } from "#bzd/nodejs/core/http/client.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import Session from "../session.mjs";

const Exception = ExceptionFactory("authentication", "session-proxy");
const Log = LogFactory("authentication", "session-proxy");

/// A server proxy authentication, only proxies the token to the authentication remote server.
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

		this.restRemote = new RestClient(APISchemaRemote.rest, {
			host: this.options.remote,
			authentication: this,
		});
	}

	/// Perform a request to the remote.
	async _requestRemote(context, ...args) {
		try {
			return await this.restRemote.request(...args);
		} catch (e) {
			// Propagate the error to the caller.
			if (e instanceof HttpClientException) {
				throw context.httpError(e.code, e.message);
			}
			throw e;
		}
	}

	_installRestImpl(rest) {
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

			// The inputs.origin is only used to detect if the proxy is not pointing to itself.
			Exception.assert(
				!inputs.origin || inputs.origin != authentication.options.remote,
				"The proxy appears to be wrongly configured and is pointing to itself: {}",
				authentication.options.remote,
			);
			inputs.origin = authentication.options.remote;

			const result = await authentication._requestRemote(this, "post", "/auth/refresh", inputs);

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
		});

		rest.handle("post", "/auth/logout", async function () {
			authentication.clearSession(this);
		});
	}

	async _verifyImpl(context) {
		const maybeToken = this._getAccessToken(context);
		if (!maybeToken) {
			return false;
		}

		const result = await this._requestRemote(context, "post", "/auth/verify", {
			token: maybeToken,
		});

		// TODO: implement caching (store the result into a database and reuse super()._verifyImpl(...))

		return new Session(result.uid, result.scopes);
	}
}
