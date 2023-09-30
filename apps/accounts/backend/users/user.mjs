import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("user");
const Log = LogFactory("user");

/// Wrapper class around a user.
export default class User {
	constructor(uid, value) {
		this.uid = uid;
		this.value = value;
		this.modified = [];
	}

	static create(uid) {
		const value = {
			creation: Date.now(),
		};
		return new User(uid, value);
	}

	clearModified() {
		this.modified = [];
	}

	getModifiedAsString() {
		return this.modified.join(", ");
	}

	getUid() {
		return this.uid;
	}

	getEmail() {
		return this.uid;
	}

	getPassword() {
		return this.value.password || null;
	}

	setPassword(password) {
		this.modified.push("password");
		this.value.password = password;
	}

	getCreationTimestamp() {
		return this.value.creation || 0;
	}

	getRoles() {
		const roles = this.value.roles || [];
		return roles;
	}

	addRole(role) {
		this.modified.push("role(+" + role + ")");

		let roles = this.value.roles || [];
		if (roles.includes(role)) {
			return;
		}
		roles.push(role);
		this.value.roles = roles;
	}

	data() {
		return this.value;
	}

	dataPublic() {
		return {
			email: this.getEmail(),
			roles: this.getRoles(),
		};
	}
}
