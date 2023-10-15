import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("subscription");
const Log = LogFactory("subscription");

/// A subscription has the following data:
///
/// duration: The duration of the subscription.
/// end: The time limit.
/// Note: duration and days cannot live together, when initiated a subscription
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

	data() {
		return this.value;
	}

	isStarted() {
		return "duration" in this.value;
	}

	start() {
		Exception.assert(!this.isStarted(), "This subscription is already started.");
		this.value.end = Date.now() + this.value.duration;
		delete this.value.duration;
	}

	stop() {
		Exception.assert(this.isStarted(), "This subscription is not started.");
		this.value.duration = this.getDuration();
		delete this.value.end;
	}

	getDuration() {
		if (this.isStarted()) {
			return this.value.duration;
		}
		return Math.min(this.value.end - Date.now(), 0);
	}

	add(subscription) {
		if (this.isStarted()) {
			this.value.end += subscription.getDuration();
		} else {
			this.value.duration += subscription.getDuration();
		}
	}
}
