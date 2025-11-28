import Clock from "#bzd/nodejs/core/clock/clock.mjs";

export default class ClockDate extends Clock {
	_getTimeMs() {
		return Date.now();
	}
}
