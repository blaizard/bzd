import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Provider from "#bzd/nodejs/core/statistics/provider.mjs";

const Exception = ExceptionFactory("statistics");
const Log = LogFactory("statistics");

/// This class is a statistics manager.
export default class Statistics {
	constructor() {
		this.providers = {};
		// Used to run processors.
		this.intervalProcessors = null;
	}

	/// Create the statistics UID from the name and the service provider.
	_makeStartisticsUid(name) {
		if (name === null) {
			let counter = 0;
			do {
				name = "_" + (++counter).toString();
			} while (this._makeStartisticsUid(name) in this.providers);
		}
		return name;
	}

	/// Register a statistic provider.
	///
	/// \param provider The statistics provider.
	/// \param name The name of the statistics.
	register(provider, name = null) {
		const uid = this._makeStartisticsUid(name);
		Exception.assert(!(uid in this.providers), "Statistics '{}' is already registered.", uid);
		this.providers[uid] = provider;
	}

	/// Create a provider and attach it to this server.
	makeProvider(name = null) {
		const provider = new Provider();
		this.register(provider, name);
		return provider;
	}

	installRest(api) {
		Log.info("Installing 'Statistics' REST.");

		api.handle("get", "/admin/statistics", async () => {
			return {
				data: Object.fromEntries(Object.entries(this.providers).map(([uid, provider]) => [uid, provider.root])),
			};
		});
	}

	/// Starting statistics.
	async start() {
		Exception.assert(this.intervalProcessors === null, "Statistics is already started.");
		this.intervalProcessors = setInterval(() => {
			for (const provider of Object.values(this.providers)) {
				for (const processors of Object.values(provider.processors)) {
					processors.process(1);
				}
			}
		}, 1000);
	}

	/// Stopping statistics.
	async stop() {
		clearInterval(this.intervalProcessors);
	}
}
