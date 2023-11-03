export default class User {
	constructor(uid, roles) {
		this.uid = uid;
		this.roles = new Set(roles);
	}

	/// Get user identifier.
	getUid() {
		return this.uid;
	}

	/// Get group associated with this user.
	getRoles() {
		return [...this.roles.values()];
	}

	/// Check if a single role is present.
	matchRole(role) {
		return this.matchAnyRoles(role);
	}

	/// Ensure that all roles are associated with this user.
	matchAllRoles(roleOrRoles) {
		const roleList = Array.isArray(roleOrRoles) ? roleOrRoles : [roleOrRoles];
		const roles = this.getRoles();
		return roleList.every((role) => roles.includes(role));
	}

	/// Ensure that at least one role is associated with this user.
	matchAnyRoles(roleOrRoles) {
		const roleList = Array.isArray(roleOrRoles) ? roleOrRoles : [roleOrRoles];
		const roles = this.getRoles();
		return roleList.some((role) => roles.includes(role));
	}

	/// Have exactly the same roles are the ones passed into argument.
	sameRolesAs(roles) {
		let rolesToCheck = [...new Set(roles).values()];
		if (rolesToCheck.length != this.roles.length) {
			return false;
		}
		while (rolesToCheck.length) {
			const role = rolesToCheck.pop();
			if (!(role in this.roles)) {
				return false;
			}
		}
		return true;
	}
}
