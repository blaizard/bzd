import Result from "#bzd/nodejs/utils/result.mjs";

export default class Scopes {
	constructor(scopes) {
		this.list_ = [...new Set(scopes).values()].filter((scope) => Boolean(scope.trim()));
	}

	toList() {
		return [...this.list_];
	}

	/// Check that this scope is contained in this object.
	contains(scope) {
		return this.list_.some(
			(objectScope) =>
				scope.startsWith(objectScope) &&
				(objectScope.at(-1) == "/" || [undefined, "/"].includes(scope[objectScope.length])),
		);
	}

	/// Ensure that at least one scope is associated with this session.
	matchAny(scopeOrScopes) {
		const scopeList = Array.isArray(scopeOrScopes) ? scopeOrScopes : [scopeOrScopes];
		return scopeList.some((scope) => this.contains(scope));
	}

	/// Have exactly the same scopes are the ones passed into argument.
	sameAs(scopes) {
		let scopesObject = new Scopes(scopes);
		if (scopesObject.list_.length != this.list_.length) {
			return false;
		}
		return scopesObject.list_.every((scope) => this.list_.includes(scope));
	}

	/// Filter a dictionary with key: [scopes...] pairs.
	///
	/// \param data The dictionary.
	/// \param keyScopesMap A dictionary key to scope correspondence map.
	/// \return The filtered dictionary.
	filterDictionary(data, keyScopesMap) {
		return Object.entries(keyScopesMap).reduce((obj, [key, scopeOrScopes]) => {
			if (this.matchAny(scopeOrScopes)) {
				obj[key] = data[key];
			}
			return obj;
		}, {});
	}

	/// Check that all dictionary keys matches the key: [scopes...] pairs.
	///
	/// \return a result containing the error message if any.
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
	///
	/// \return a result containing the error message if any.
	checkValid(scopes) {
		for (const scope of scopes) {
			if (!this.contains(scope)) {
				return Result.makeErrorString("Scope '{}' is not valid, must be one of: {:j}.", scope, this.list_);
			}
		}
		return new Result();
	}
}
