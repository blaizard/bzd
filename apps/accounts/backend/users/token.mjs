import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("token");
const Log = LogFactory("token");

/// A token has the following data:
///
/// roles: The roles associated with this token.
/// expiration: The expiration date.
/// rolling: Wether it is a rolling token or not.
export default class TokenInfo {
	constructor(value) {
		Exception.assert("expiration" in value, "A token must have 'expiration' in its attributes: {}", value);
		this.value = value;
	}

	static make(roles, timeoutS, rolling) {
		return new TokenInfo({
			roles: roles,
			expiration: Date.now() + timeoutS * 1000,
			rolling: rolling,
		});
	}

	data() {
		return this.value;
	}

	isRolling() {
		return this.value.rolling || false;
	}

	isExpired() {
		return Date.now() > this.value.expiration;
	}

	getRoles() {
		return this.value.roles || [];
	}
}