import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("application");
const Log = LogFactory("application");

/// Wrapper class around an application.
export default class Application {
	constructor(uid, value) {
		this.uid = uid;
		this.value = value;
		this.modified = [];
	}

	static create(uid, redirect) {
		const value = {
			creation: Date.now(),
			redirect: redirect,
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
		};
	}
}
