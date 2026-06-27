import Clock from "#bzd/nodejs/core/clock/clock.js";

export default class ClockMock extends Clock {
	constructor() {
		super();
		this.timeMs = 0;
	}

	_getTimeMs() {
		return this.timeMs;
	}

	advanceMs(offset) {
		this.timeMs += offset;
	}
}
