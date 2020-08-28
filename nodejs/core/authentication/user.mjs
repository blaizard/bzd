export default class User {
	constructor(uid, roles) {
		this.uid = uid;
		this.roles = roles;
	}

	/**
	 * Get user identifier.
	 */
	getUid() {
		return this.uid;
	}

	/**
	 * Get grousp associated with this user.
	 */
	async getRoles() {
		return this.roles;
	}
}
