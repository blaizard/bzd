import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Scopes from "#bzd/nodejs/core/authentication/scopes.mjs";

import {
	scopeAdmin,
	scopeSelfBasicRead,
	scopeSelfBasicWrite,
	scopeSelfSubscriptionsRead,
	scopeSelfSubscriptionsCancel,
} from "#bzd/apps/accounts/backend/users/scopes.mjs";

const Exception = ExceptionFactory("roles-scopes");
const Log = LogFactory("roles-scopes");

const rolesScopesMap = {
	admin: new Set([scopeAdmin]),
	user: new Set([scopeSelfBasicRead, scopeSelfBasicWrite, scopeSelfSubscriptionsRead, scopeSelfSubscriptionsCancel]),
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
		let scopes = [];
		for (const role of roles) {
			Exception.assert(role in rolesScopesMap, "There is no role named '{}'.", role);
			scopes = [...scopes, ...rolesScopesMap[role]];
		}

		return new Scopes(scopes);
	}
}
