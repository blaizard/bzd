import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("roles-scopes");
const Log = LogFactory("roles-scopes");

const rolesScopesMap = {
	admin: new Set(["users:r", "users:w", "subscription:a"]),
	user: new Set(["self-basic:r", "self-basic:w", "self-subscription:r", "self-subscription:w"]),
};

export default class Roles {
	static getScopes(...roles) {
		let scopes = new Set();
		for (const role of roles) {
			Exception.assert(role in rolesScopesMap, "There is no role named '{}'.", role);
			scopes = new Set([...scopes, ...rolesScopesMap[role]]);
		}

		return [...scopes];
	}
}
