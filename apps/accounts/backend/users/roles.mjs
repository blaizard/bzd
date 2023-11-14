import API from "#bzd/apps/accounts/backend/users/api.json" assert { type: "json" };
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

import {
	scopeAdminUserRead,
	scopeAdminUserWrite,
	scopeSelfBasicRead,
	scopeSelfBasicWrite,
	scopeSelfRolesRead,
	scopeSelfRolesWrite,
	scopeSelfTokensRead,
	scopeSelfTokensWrite,
	scopeSelfSubscriptionsRead,
	scopeSelfSubscriptionsWrite,
} from "#bzd/apps/accounts/backend/users/scopes.mjs";

const Exception = ExceptionFactory("roles-scopes");
const Log = LogFactory("roles-scopes");

const rolesScopesMap = {
	admin: new Set([
		scopeAdminUserRead,
		scopeAdminUserWrite,
		scopeSelfRolesRead,
		scopeSelfRolesWrite,
		scopeSelfTokensWrite,
		scopeSelfSubscriptionsWrite,
	]),
	user: new Set([scopeSelfBasicRead, scopeSelfBasicWrite, scopeSelfSubscriptionsRead, scopeSelfTokensRead]),
};

export default class Roles {
	/// Get all roles.
	static all() {
		return Object.keys(rolesScopesMap);
	}

	/// Check if a role is valid.
	static isValid(role) {
		return Roles.all().includes(role);
	}

	/// Get the scopes associated with the roles.
	static getScopes(...roles) {
		let scopes = new Set();
		for (const role of roles) {
			Exception.assert(role in rolesScopesMap, "There is no role named '{}'.", role);
			scopes = new Set([...scopes, ...rolesScopesMap[role]]);
		}

		return [...scopes];
	}
}
