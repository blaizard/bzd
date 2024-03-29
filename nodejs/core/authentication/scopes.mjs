import Result from "#bzd/nodejs/utils/result.mjs";

export default class Scopes {
	constructor(scopes) {
		this.values_ = new Set(scopes);
	}

	toList() {
		return [...this.values_.values()];
	}

	/// Check that this scope is contained in this object.
	contains(scope) {
		return this.values_.has(scope);
	}

	/// Ensure that all scopes are associated with this.
	matchAll(scopeOrScopes) {
		const scopeList = Array.isArray(scopeOrScopes) ? scopeOrScopes : [scopeOrScopes];
		return scopeList.every((scope) => this.values_.has(scope));
	}

	/// Ensure that at least one scope is associated with this session.
	matchAny(scopeOrScopes) {
		const scopeList = Array.isArray(scopeOrScopes) ? scopeOrScopes : [scopeOrScopes];
		return scopeList.some((scope) => this.values_.has(scope));
	}

	/// Have exactly the same scopes are the ones passed into argument.
	sameAs(scopeOrScopes) {
		let scopeList = Array.isArray(scopeOrScopes) ? [...new Set(scopeOrScopes).values()] : [scopeOrScopes];
		if (scopeList.length != this.values_.length) {
			return false;
		}
		while (scopeList.length) {
			const scope = scopeList.pop();
			if (!this.values_.has(scope)) {
				return false;
			}
		}
		return true;
	}

	/// Filter a dictionary with key: [scopes...] pairs.
	///
	/// \param data The dictionary.
	/// \param keyScopesMap A dictionary key to scope correspondence map.
	/// \return The filtered dictionary.
	filterDictionary(data, keyScopesMap) {
		return Object.entries(keyScopesMap).reduce((obj, [key, scopes]) => {
			if (this.matchAny(scopes)) {
				obj[key] = data[key];
			}
			return obj;
		}, {});
	}

	/// Check that all dictionary keys matches the key: [scopes...] pairs.
	checkDictionary(data, keyScopesMap) {
		const allKeys = new Set([...Object.keys(data), ...Object.keys(keyScopesMap)]);
		for (const key of allKeys) {
			if (key in data && key in keyScopesMap) {
				if (!this.matchAny(keyScopesMap[key])) {
					return Result.makeErrorString("Key '{}' is not valid for this scope.", key);
				}
			} else if (key in keyScopesMap) {
				// ignore, no need to handle keys that are only set by the scopes map and not in the actual data.
			} else {
				return Result.makeErrorString("Key '{}' is set but not guarded by a scope.", key);
			}
		}
		return new Result();
	}

	/// Check that all the scopes passed into arguments are valid (part of this).
	checkValid(scopes) {
		for (const scope of scopes) {
			if (!this.values_.has(scope)) {
				return Result.makeErrorString("Scope '{}' is not valid, must be one of: {}.", scope, [...this.values_]);
			}
		}
		return new Result();
	}

	intersect(scopes) {
		return new Scopes([...scopes].filter((scope) => this.values_.has(scope)));
	}
}
