export default class User {
  constructor(uid, roles) {
	this.uid = uid;
	this.roles = roles;
  }

  /**
   * Get user identifier.
   */
  getUid() { return this.uid; }

  /**
   * Get grousp associated with this user.
   */
  async getRoles() { return this.roles; }

  /**
   * Check if a single role is present.
   */
  async matchRole(role) { return await this.matchAnyRoles(role); }

  /**
   * Ensure that all roles are associated with this user.
   */
  async matchAllRoles(roleOrRoles) {
	const roleList = Array.isArray(roleOrRoles) ? roleOrRoles : [ roleOrRoles ];
	const roles = await this.getRoles();
	return roleList.every((role) => roles.includes(role));
  }

  /**
   * Ensure that at least one role is associated with this user.
   */
  async matchAnyRoles(roleOrRoles) {
	const roleList = Array.isArray(roleOrRoles) ? roleOrRoles : [ roleOrRoles ];
	const roles = await this.getRoles();
	return roleList.some((role) => roles.includes(role));
  }
}
