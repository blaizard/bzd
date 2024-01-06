import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Subscription from "#bzd/apps/accounts/backend/users/subscription.mjs";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.mjs";
import Roles from "#bzd/apps/accounts/backend/users/roles.mjs";
import Bcrypt from "#bzd/nodejs/core/cryptography/bcrypt.mjs";

const Exception = ExceptionFactory("user");
const Log = LogFactory("user");

const VALID_SUBSCRIPTIONS_ = ["screen_recorder"];

/// Wrapper class around a user.
export default class User {
	constructor(uid, value) {
		this.uid = uid;
		this.value = value;
		this.modified = [];
	}

	static createDummy() {
		const value = {
			creation: Date.now(),
			email: "dummy@dummy.com",
		};
		return new User(1234, value);
	}

	static create(uid, email) {
		const value = {
			creation: Date.now(),
			email: email,
		};
		return new User(uid, value);
	}

	clearModified() {
		this.modified = [];
	}

	getModifiedAsString() {
		return this.modified.join(", ");
	}

	getUid() {
		return this.uid;
	}

	getEmail() {
		return this.value.email;
	}

	getPassword() {
		return this.value.password || null;
	}

	setLastLogin() {
		this.modified.push("last_login");
		this.value.last_login = Date.now();
	}

	setLastFailedLogin() {
		this.modified.push("last_failed_login");
		this.value.last_failed_login = Date.now();
	}

	async setPassword(password) {
		this.modified.push("password");
		const hash = await Bcrypt.hash(password);
		this.value.password = hash;
	}

	async isPasswordEqual(password) {
		if (!this.value.password || !password) {
			return false;
		}
		return await Bcrypt.compare(this.value.password, password);
	}

	getCreationTimestamp() {
		return this.value.creation || 0;
	}

	getLastLoginTimestamp() {
		return this.value.last_login || 0;
	}

	getLastFailedLoginTimestamp() {
		return this.value.last_failed_login || 0;
	}

	getRoles() {
		return this.value.roles || [];
	}

	// Get all the scopes from a user.
	getScopes() {
		return Roles.getScopes(...this.getRoles());
	}

	hasRole(role) {
		return this.getRoles().includes(role);
	}

	hasScope(scope) {
		return this.getScopes().contains(scope);
	}

	addRole(role) {
		Exception.assert(Roles.isValid(role), "'{}' is not a valid role, valid roles are: {}", role, Roles.all());
		this.modified.push("role(+" + role + ")");

		let roles = this.value.roles || [];
		if (roles.includes(role)) {
			return;
		}
		roles.push(role);
		this.value.roles = roles;
	}

	setRoles(roles) {
		this.modified.push("role()");
		this.value.roles = [];
		for (const role of roles) {
			this.addRole(role);
		}
	}

	getSubscriptions() {
		return this.value.subscriptions || {};
	}

	/// Get the subscription associated with a specific product.
	///
	/// \product The product ID.
	getSubscription(product) {
		const subscriptions = this.getSubscriptions();
		if (product in subscriptions) {
			return new Subscription(subscriptions[product]);
		}
		return null;
	}

	addSubscription(product, subscription) {
		Exception.assert(
			VALID_SUBSCRIPTIONS_.includes(product),
			"'{}' is not a valid subscription product, valid products are: {}",
			product,
			VALID_SUBSCRIPTIONS_,
		);
		this.modified.push("subscriptions(+" + product + ")");

		if (!("subscriptions" in this.value)) {
			this.value.subscriptions = {};
		}

		if (product in this.value.subscriptions) {
			let original = new Subscription(this.value.subscriptions[product]);
			original.add(subscription);
		} else {
			this.value.subscriptions[product] = subscription.data();
		}
	}

	setSubscriptions(subscriptions) {
		let user = User.createDummy();
		for (const [product, data] of Object.entries(subscriptions)) {
			const subscription = new Subscription(data);
			user.addSubscription(product, subscription);
		}

		this.value.subscriptions = user.getSubscriptions();
		this.modified.push("subscriptions()");
		this.modified.push(...user.modified);
	}

	getTokens() {
		return this.value.tokens || {};
	}

	/// Set all the tokens of a user.
	setTokens(tokens) {
		let user = User.createDummy();
		user.value.roles = this.getRoles();
		for (const [hash, data] of Object.entries(tokens)) {
			const token = new TokenInfo(data);
			user.addToken(hash, token);
		}

		this.value.tokens = user.getTokens();
		this.modified.push("tokens()");
		this.modified.push(...user.modified);
	}

	/// Add a token to the user.
	addToken(hash, token) {
		if (!("tokens" in this.value)) {
			this.value.tokens = {};
		}
		Exception.assert(
			!(hash in this.value.tokens),
			"Token with hash '{}' already exists for user '{}'.",
			hash,
			this.uid,
		);
		Exception.assert(token instanceof TokenInfo, "Token must be of type TokenInfo: '{}'.", token);
		Exception.assertResult(this.getScopes().checkValid(token.getScopes().toList()));

		this.modified.push("tokens(" + token.identifier() + " +" + hash.slice(0, 16) + "[...])");
		this.value.tokens[hash] = token.data();
	}

	getToken(hash, defaultValue) {
		if ("tokens" in this.value) {
			if (hash in this.value.tokens) {
				return new TokenInfo(this.value.tokens[hash]);
			}
		}
		return defaultValue;
	}

	removeToken(hash) {
		if ("tokens" in this.value) {
			if (hash in this.value.tokens) {
				const token = this.getToken(hash, null);
				Exception.assert(token !== null, "There is no token with this hash: '{}'.", hash);
				this.modified.push("tokens(" + token.identifier() + " -" + hash.slice(0, 16) + "[...])");
				delete this.value.tokens[hash];
			}
		}
	}

	/// Only keep non-expired tokens.
	sanitizedTokens() {
		this.value.tokens = Object.fromEntries(
			Object.entries(this.getTokens()).filter(([hash, data]) => {
				const token = new TokenInfo(data);
				return !token.isExpired();
			}),
		);
	}

	rollToken(hash, newHash) {
		const token = this.getToken(hash, null);
		Exception.assert(token !== null, "There is no token with this hash: '{}'.", hash);
		Exception.assert(this.getToken(newHash, null) === null, "There is already a token with this hash: '{}'.", newHash);

		this.modified.push(
			"tokens(" + token.identifier() + " -" + hash.slice(0, 16) + "[...] +" + newHash.slice(0, 16) + "[...])",
		);
		delete this.value.tokens[hash];
		this.value.tokens[newHash] = token.data();

		return token;
	}

	data() {
		return this.value;
	}

	dataPublic() {
		return {
			email: this.getEmail(),
			creation: this.getCreationTimestamp(),
			last_login: this.getLastLoginTimestamp(),
			last_failed_login: this.getLastFailedLoginTimestamp(),
			roles: this.getRoles(),
			scopes: this.getScopes().toList(),
			subscriptions: this.getSubscriptions(),
			tokens: this.getTokens(),
		};
	}
}
