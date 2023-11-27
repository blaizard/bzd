import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Scopes from "#bzd/nodejs/core/authentication/scopes.mjs";
import { allScopes } from "#bzd/apps/accounts/backend/users/scopes.mjs";

const Exception = ExceptionFactory("application");
const Log = LogFactory("application");

/// Wrapper class around an application.
export default class Application {
	constructor(uid, value) {
		this.uid = uid;
		this.value = value;
		this.modified = [];
	}

	static create(uid, redirect, scopes) {
		Exception.assertPreconditionResult(allScopes.checkValid(scopes));
		const value = {
			creation: Date.now(),
			redirect: redirect,
			scopes: scopes,
		};
		return new Application(uid, value);
	}

	/// Get the UID associated with this application configuration.
	getUid() {
		return this.value.product;
	}

	/// Get the redirect URL.
	getRedirect() {
		return this.value.redirect;
	}

	/// Get the scopes.
	getScopes() {
		return new Scopes(this.value.scopes || []);
	}

	getCreationTimestamp() {
		return this.value.creation || 0;
	}

	data() {
		return this.value;
	}

	dataPublic() {
		return {
			creation: this.getCreationTimestamp(),
			redirect: this.getRedirect(),
			scopes: this.getScopes().toList(),
		};
	}
}
