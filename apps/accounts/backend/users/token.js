import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Scopes from "#bzd/nodejs/core/authentication/scopes.js";

const Exception = ExceptionFactory("token");
const Log = LogFactory("token");

const defaultClock = { getTimeMs: () => Date.now() };

/// A token has the following data:
///
/// scopes: The scopes associated with this token.
/// expiration: The expiration date.
/// rolling: Whether it is a rolling token or not.
export default class TokenInfo {
	constructor(value, clock = defaultClock) {
		Exception.assert("expiration" in value, "A token must have 'expiration' in its attributes: {}", value);
		this.value = value;
		this.clock = clock;
	}

	static make(identifier, scopes, timeoutS, rolling, clock = defaultClock) {
		const nowMs = clock.getTimeMs();
		return new TokenInfo(
			{
				identifier: identifier,
				scopes: scopes,
				creation: nowMs,
				expiration: nowMs + timeoutS * 1000,
				lifetime: timeoutS,
				rolling: rolling,
			},
			clock,
		);
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

	// Update the minimal duration of this token.
	// The extension is bounded by the lifetime the token was issued with, so that a
	// single refresh can never exceed the TTL of its class (SSO/login refresh tokens).
	updateMinDuration(minDuration) {
		const lifetime = this.value.lifetime ?? (this.value.expiration - this.value.creation) / 1000;
		const bounded = Math.min(minDuration, lifetime);
		if (this.duration() < bounded) {
			this.value.expiration = this.clock.getTimeMs() + bounded * 1000;
		}
	}

	// Duration left for this token.
	duration() {
		return Math.max((this.value.expiration - this.clock.getTimeMs()) / 1000, 0);
	}

	isExpired() {
		return this.clock.getTimeMs() > this.value.expiration;
	}

	getScopes() {
		return new Scopes(this.value.scopes || []);
	}
}
