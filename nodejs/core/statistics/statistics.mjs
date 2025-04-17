import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("statistics");
const Log = LogFactory("statistics");

/// This class is a statistics manager.
export default class Statistics {
	constructor() {
		this.data = {};
	}

	/// Create the statistics UID from the name and the service provider.
	_makeStartisticsUid(name, provider) {
		if (name === null) {
			let counter = 0;
			do {
				name = (++counter).toString();
			} while (this._makeStartisticsUid(name, provider) in this.data);
		}
		return provider.namespace.join(".") + "." + name;
	}

	/// Register a statistic provider.
	///
	/// \param provider The statistics provider.
	/// \param name The name of the statistics.
	register(provider, name = null) {
		const uid = this._makeStartisticsUid(name, provider);
		Exception.assert(!(uid in this.data), "Statistics '{}' is already registered.", uid);
		this.data[uid] = provider;
	}

	installRest(api) {
		Log.info("Installing 'Statistics' REST.");
	}
}
