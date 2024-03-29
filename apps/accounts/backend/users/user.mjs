import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Subscription from "#bzd/apps/accounts/backend/users/subscription.mjs";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.mjs";
import Roles from "#bzd/apps/accounts/backend/users/roles.mjs";
import Bcrypt from "#bzd/nodejs/core/cryptography/bcrypt.mjs";

const Exception = ExceptionFactory("user");
const Log = LogFactory("user");

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

	setLastPasswordReset() {
		this.modified.push("last_password_reset");
		this.value.last_password_reset = Date.now();
	}

	async setPassword(password) {
		this.modified.push("password");
		const hash = await Bcrypt.hash(password);
		this.value.password = hash;
	}

	async setRandomPassword() {
		await this.setPassword(Math.random().toString());
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

	getLastPasswordResetTimestamp() {
		return this.value.last_password_reset || 0;
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

	/// Get all the payment references already processed for this user.
	getPayments() {
		return this.value.payments || [];
	}

	/// Check if a specific payment has been processed.
	hasPayment(uid) {
		return this.getPayments().includes(uid);
	}

	/// Register a payment reference processed by this user.
	///
	/// Those are payment already done, that do not need further processing.
	registerPayment(uid) {
		Exception.assert(!this.hasPayment(uid), "The payment '{}' was already registered.", uid);
		if (!("payments" in this.value)) {
			this.value.payments = [];
		}
		this.value.payments.push(uid);
	}

	getSubscriptions() {
		return this.value.subscriptions || {};
	}

	/// Get the subscription associated with a specific application.
	///
	/// \param application The application UID.
	/// \param allowNull If the result can be null or not.
	getSubscription(application, allowNull = false) {
		const subscriptions = this.getSubscriptions();
		if (application in subscriptions) {
			return new Subscription(subscriptions[application]);
		}
		Exception.assert(allowNull, "The subscription for application '{}' is not present.", application);
		return null;
	}

	addSubscription(application, subscription) {
		this.modified.push("subscriptions(+" + application + ")");

		if (!("subscriptions" in this.value)) {
			this.value.subscriptions = {};
		}

		if (application in this.value.subscriptions) {
			let original = new Subscription(this.value.subscriptions[application]);
			original.add(subscription);
		} else {
			this.value.subscriptions[application] = subscription.data();
		}
	}

	setSubscriptions(subscriptions) {
		let user = User.createDummy();
		for (const [application, data] of Object.entries(subscriptions)) {
			const subscription = new Subscription(data);
			user.addSubscription(application, subscription);
		}

		this.value.subscriptions = user.getSubscriptions();
		this.modified.push("subscriptions()");
		this.modified.push(...user.modified);
	}

	stopRecurringSubscription(uid) {
		for (const [application, data] of Object.entries(this.getSubscriptions())) {
			const subscription = new Subscription(data);
			if (subscription.stopRecurringSubscription(uid)) {
				this.modified.push("subscriptions(" + application + ":-recurring)");
			}
		}
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
			last_password_reset: this.getLastPasswordResetTimestamp(),
			payments: this.getPayments(),
			roles: this.getRoles(),
			scopes: this.getScopes().toList(),
			subscriptions: this.getSubscriptions(),
			tokens: this.getTokens(),
		};
	}
}
