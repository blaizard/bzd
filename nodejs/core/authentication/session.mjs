import Scopes from "#bzd/nodejs/core/authentication/scopes.mjs";

export default class User {
	constructor(uid, scopes) {
		this.uid = uid;
		this.scopes = new Scopes(scopes);
	}

	/// Get session identifier.
	getUid() {
		return this.uid;
	}

	/// Get scopes associated with this session.
	getScopes() {
		return this.scopes;
	}
}
