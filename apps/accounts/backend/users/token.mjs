import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Scopes from "#bzd/nodejs/core/authentication/scopes.mjs";

const Exception = ExceptionFactory("token");
const Log = LogFactory("token");

/// A token has the following data:
///
/// scopes: The scopes associated with this token.
/// expiration: The expiration date.
/// rolling: Whether it is a rolling token or not.
export default class TokenInfo {
	constructor(value) {
		Exception.assert("expiration" in value, "A token must have 'expiration' in its attributes: {}", value);
		this.value = value;
	}

	static make(identifier, scopes, timeoutS, rolling) {
		return new TokenInfo({
			identifier: identifier,
			scopes: scopes,
			creation: Date.now(),
			expiration: Date.now() + timeoutS * 1000,
			rolling: rolling,
		});
	}

	data() {
		return this.value;
	}

	// Identifier for this token.
	identifier() {
		return this.value.identifier || "<no-id>";
	}

	isRolling() {
		return this.value.rolling || false;
	}

	// Update the minimal duration of this token
	updateMinDuration(minDuration) {
		if (this.duration() < minDuration) {
			this.value.expiration = Date.now() + minDuration * 1000;
		}
	}

	// Duration left for this token.
	duration() {
		return Math.max((this.value.expiration - Date.now()) / 1000, 0);
	}

	isExpired() {
		return Date.now() > this.value.expiration;
	}

	getScopes() {
		return new Scopes(this.value.scopes || []);
	}
}
