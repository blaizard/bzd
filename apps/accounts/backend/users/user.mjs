import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Subscription from "#bzd/apps/accounts/backend/users/subscription.mjs";

const Exception = ExceptionFactory("user");
const Log = LogFactory("user");

const VALID_ROLES_ = ["admin", "user"];
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
		};
		return new User(1234, value);
	}

	static create(uid) {
		const value = {
			creation: Date.now(),
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
		return this.uid;
	}

	getPassword() {
		return this.value.password || null;
	}

	setLastLogin() {
		this.modified.push("last_login");
		this.value.last_login = Date.now();
	}

	setPassword(password) {
		this.modified.push("password");
		this.value.password = password;
	}

	getCreationTimestamp() {
		return this.value.creation || 0;
	}

	getLastLoginTimestamp() {
		return this.value.last_login || 0;
	}

	getRoles() {
		return this.value.roles || [];
	}

	addRole(role) {
		Exception.assert(VALID_ROLES_.includes(role), "'{}' is not a valid role, valid roles are: {}", role, VALID_ROLES_);
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
		this.modified.push("subscription(+" + product + ")");

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

		this.value.subscriptions = user.value.subscriptions;
		this.modified.push("subscription()");
		this.modified.push(...user.modified);
	}

	data() {
		return this.value;
	}

	dataPublic() {
		return {
			email: this.getEmail(),
			creation: this.getCreationTimestamp(),
			last_login: this.getLastLoginTimestamp(),
			roles: this.getRoles(),
			subscriptions: this.getSubscriptions(),
		};
	}
}
