import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Provider from "#bzd/nodejs/core/statistics/provider.mjs";

const Exception = ExceptionFactory("statistics");
const Log = LogFactory("statistics");

/// This class is a statistics manager.
export default class Statistics {
	constructor() {
		this.data = {};
	}

	/// Create the statistics UID from the name and the service provider.
	_makeStartisticsUid(name) {
		if (name === null) {
			let counter = 0;
			do {
				name = "_" + (++counter).toString();
			} while (this._makeStartisticsUid(name) in this.data);
		}
		return name;
	}

	/// Register a statistic provider.
	///
	/// \param provider The statistics provider.
	/// \param name The name of the statistics.
	register(provider, name = null) {
		const uid = this._makeStartisticsUid(name);
		Exception.assert(!(uid in this.data), "Statistics '{}' is already registered.", uid);
		this.data[uid] = provider;
	}

	/// Create a provider and attach it to this server.
	makeProvider(name) {
		const provider = new Provider();
		this.register(provider, name);
		return provider;
	}

	installRest(api) {
		Log.info("Installing 'Statistics' REST.");

		api.handle("get", "/admin/statistics", async () => {
			return {
				data: this.data,
			};
		});
	}
}
