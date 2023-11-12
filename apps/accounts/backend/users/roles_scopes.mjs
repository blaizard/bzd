import API from "#bzd/apps/accounts/backend/users/api.json" assert { type: "json" };
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Session from "#bzd/nodejs/core/authentication/session.mjs";

const Exception = ExceptionFactory("roles-scopes");
const Log = LogFactory("roles-scopes");

const rolesScopesMap = {
	admin: new Set([
		"admin-users:r",
		"admin-users:w",
		"self-roles:r",
		"self-roles:w",
		"self-tokens:w",
		"self-subscriptions:w",
	]),
	user: new Set(["self-basic:r", "self-basic:w", "self-subscriptions:r", "self-tokens:r"]),
};

Exception.assertResult(Session.checkValidScopes(rolesScopesMap.admin, Object.keys(API.scopes)));
Exception.assertResult(Session.checkValidScopes(rolesScopesMap.user, Object.keys(API.scopes)));

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
