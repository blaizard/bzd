import Clock from "#bzd/nodejs/core/clock/clock.js";

export default class ClockDate extends Clock {
	_getTimeMs() {
		return Date.now();
	}
}
