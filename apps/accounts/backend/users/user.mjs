import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("users");
const Log = LogFactory("users");

class User {
	constructor(uid, value) {
		this.uid = uid;
		this.value = value;
	}

	getUid() {
		return this.uid;
	}

	getEmail() {
		return this.uid;
	}

	data() {
		return Object.assign(
			{
				creation: this.value.creation || 0,
				password: this.getPassword(),
			},
			this.dataPublic()
		);
	}

	dataPublic() {
		return {
			email: this.getEmail(),
			roles: this.getRoles()
		};
	}

	getPassword() {
		return this.value.password || null;
	}

	getRoles() {
		const roles = this.value.roles || [];
		return roles;
	}
}
