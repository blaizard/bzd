import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("clock");

export default class Clock {
	constructor() {
		Exception.assert(typeof this._getTime === "function", "A clock must implement a '_getTime' function.");
	}

	/// Will be called upon startup.
	async start() {}

	/// Will be called upon destruction.
	async stop() {}

	/// Get the current timestamp in milliseconds.
	///
	/// \return The timestamp in milliseconds.
	getTime() {
		return this._getTime();
	}
}
