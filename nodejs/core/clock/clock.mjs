import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("clock");

export default class Clock {
	constructor() {
		Exception.assert(typeof this._getTimeMs === "function", "A clock must implement a '_getTimeMs' function.");
	}

	/// Will be called upon startup.
	async start() {}

	/// Will be called upon destruction.
	async stop() {}

	/// Get the current timestamp in milliseconds.
	///
	/// \return The timestamp in milliseconds.
	getTimeMs() {
		return this._getTimeMs();
	}

	/// Get the current timestamp in seconds.
	///
	/// \return The timestamp in seconds.
	getTimeS() {
		return this._getTimeMs() / 1000;
	}
}
