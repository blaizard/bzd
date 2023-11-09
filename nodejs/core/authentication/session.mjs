import Result from "#bzd/nodejs/utils/result.mjs";

export default class User {
	constructor(uid, scopes) {
		this.uid = uid;
		this.scopes = new Set(scopes);
	}

	/// Get user identifier.
	getUid() {
		return this.uid;
	}

	/// Get scopes associated with this user.
	getScopes() {
		return [...this.scopes.values()];
	}

	/// Ensure that all scopes are associated with this user.
	matchAllScopes(scopeOrScopes) {
		const scopeList = Array.isArray(scopeOrScopes) ? scopeOrScopes : [scopeOrScopes];
		const scopes = this.getScopes();
		return scopeList.every((scope) => scopes.includes(scope));
	}

	/// Ensure that at least one scope is associated with this user.
	matchAnyScopes(scopeOrScopes) {
		const scopeList = Array.isArray(scopeOrScopes) ? scopeOrScopes : [scopeOrScopes];
		const scopes = this.getScopes();
		return scopeList.some((scope) => scopes.includes(scope));
	}

	/// Have exactly the same scopes are the ones passed into argument.
	sameScopesAs(scopeOrScopes) {
		let scopesToCheck = Array.isArray(scopeOrScopes) ? [...new Set(scopeOrScopes).values()] : [scopeOrScopes];
		if (scopesToCheck.length != this.scopes.length) {
			return false;
		}
		while (scopesToCheck.length) {
			const scope = scopesToCheck.pop();
			if (!(scope in this.scopes)) {
				return false;
			}
		}
		return true;
	}

	/// Filter a dictionary with key: [scopes...] pairs.
	filterByScopes(data, keyScopesMap) {
		return Object.entries(keyScopesMap).reduce((obj, [key, scopes]) => {
			if (this.matchAnyScopes(scopes)) {
				obj[key] = data[key];
			}
			return obj;
		}, {});
	}

	/// Check that all dictionary keys matchs the key: [scopes...] pairs.
	checkAllByScopes(data, keyScopesMap) {
		const allKeys = new Set([...Object.keys(data), ...Object.keys(keyScopesMap)]);
		for (const key of allKeys) {
			if (key in data && key in keyScopesMap) {
				if (this.matchAnyScopes(keyScopesMap[key])) {
					return Result.makeErrorString("Key '{}' is not valid for this session scope.", key);
				}
			} else if (key in keyScopesMap) {
				if (!this.matchAnyScopes(keyScopesMap[key])) {
					return Result.makeErrorString("Key '{}' is expected but expected.", key);
				}
			} else {
				return Result.makeErrorString("Key '{}' is set but not guarded by a session scope.", key);
			}
		}
		return new Result();
	}
}
