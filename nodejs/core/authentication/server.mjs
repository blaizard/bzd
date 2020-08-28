import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("authentication", "server");

export default class AuthenticationServer {
	constructor(options, defaultOptions = {}) {
		this.options = Object.assign(
			{
				/**
				 * Callback to verify once identiy, mathing uid and password pair.
				 */
				verifyIdentityAndGetRolesCallback: null,
				verifyRefreshCallback: null
			},
			defaultOptions,
			options
		);
	}

	installAPI(api) {
		return this._installAPIImpl(api);
	}

	async verify(request, callback = async (/*user*/) => true) {
		return this._verifyImpl(request, callback);
	}

	async verifyIdentityAndGetRoles(uid, password) {
		Exception.assert(this.options.verifyIdentityAndGetRolesCallback, "verifyIdentityAndGetRolesCallback is not set.");
		return await this.options.verifyIdentityAndGetRolesCallback(uid, password);
	}

	async verifyRefresh(uid, session) {
		Exception.assert(this.options.verifyRefreshCallback, "verifyRefreshCallback is not set.");
		return await this.options.verifyRefreshCallback(uid, session);
	}
}
