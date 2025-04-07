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

		const authentication = this;
		// The refreshToken calls directly the server.
		// Note, this assignation cannot be done before, because 'super()' needs to be called before we
		// can access 'this'.
		this.options.refreshToken = async function (uid, hash, timeout, hashNext) {
			return await authentication._requestRemote(this, "post", "/auth/refresh_token", {
				uid: uid,
				hash: hash,
				timeout: timeout,
				next: hashNext,
			});
		};

		Log.info("Using session proxy authentication with server {}.", this.options.remote);
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

		const authentication = this;
		rest.handle("post", "/auth/refresh", async function (inputs) {
			return await authentication.refresh(this, inputs.refresh_token);
		});

		rest.handle("post", "/auth/logout", async function () {
			authentication.clearSession(this);
		});
	}
}
