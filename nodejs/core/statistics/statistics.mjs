import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("statistics");
const Log = LogFactory("statistics");

/// This class is a statistics manager.
export default class Statistics {
	/// Register a startistic provider.
	///
	/// \param provider The statistics provider.
	/// \param name The name of the statistics.
	register(provider, name = null) {}

	installRest(api) {
		Log.info("Installing 'Statistics' REST.");
	}
}
