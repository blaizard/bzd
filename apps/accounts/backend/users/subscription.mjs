import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("subscription");
const Log = LogFactory("subscription");

/// A subscription has the following data:
///
/// duration: The duration of the subscription.
/// end: The time limit.
/// Note: duration and end cannot live together, when initiated a subscription
/// has a duration only and when started, the days will also start.
export default class Subscription {
	constructor(value) {
		Exception.assert(
			"end" in value != "duration" in value,
			"A subscription must have 'end' or 'duration' in its attributes but not both: {}",
			value,
		);
		this.value = value;
	}

	/// Create an active subscription.
	static makeFromTimestamp(timestampMs) {
		const subscription = new Subscription({ end: timestampMs });
		return subscription;
	}

	/// Create an inactive subscription.
	static makeFromDuration(durationMs) {
		const subscription = new Subscription({ duration: durationMs });
		return subscription;
	}

	data() {
		return this.value;
	}

	isStarted() {
		return "end" in this.value;
	}

	isExpired() {
		return this.getDuration() == 0;
	}

	isActive() {
		return this.isStarted() && !this.isExpired();
	}

	start() {
		if (!this.isStarted()) {
			this.value.end = Date.now() + this.value.duration;
			delete this.value.duration;
		}
	}

	stop() {
		if (this.isStarted()) {
			this.value.duration = this.getDuration();
			delete this.value.end;
		}
	}

	getDuration() {
		if (this.isStarted()) {
			return Math.max(this.value.end - Date.now(), 0);
		}
		return this.value.duration;
	}

	add(subscription) {
		if (this.isStarted()) {
			this.value.end += subscription.getDuration();
		} else {
			this.value.duration += subscription.getDuration();
		}
		// Add the recurring subscriptions.
		this.value.recurring = Object.assign(this.value.recurring || {}, subscription.getRecurringSubscriptions());
		this.sanitize();
	}

	/// Set or overwrite a recurring subscription.
	addRecurringSubscription(uid, endMs) {
		if (!("recurring" in this.value)) {
			this.value.recurring = {};
		}
		this.value.recurring[uid] = endMs;
	}

	getRecurringSubscriptions() {
		return this.value.recurring || {};
	}

	stopRecurringSubscription(uid) {
		if (uid in this.getRecurringSubscriptions()) {
			delete this.value.recurring[uid];
			return true;
		}
		return false;
	}

	/// Clear all expired recurring subscriptions.
	sanitize() {
		this.value.recurring = Object.entries(this.getRecurringSubscriptions()).reduce((recurring, [uid, endMs]) => {
			if (endMs > Date.now()) {
				recurring[uid] = endMs;
			}
			return recurring;
		}, {});
	}
}
