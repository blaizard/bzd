import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Provider from "#bzd/nodejs/core/statistics/provider.js";
import Data from "#bzd/nodejs/db/data/data.js";
import { handleDataGet } from "#bzd/nodejs/db/data/backend/handler.js";

const Exception = ExceptionFactory("statistics");
const Log = LogFactory("statistics");

/// This class is a statistics manager.
export default class Statistics {
	constructor() {
		this.data = new Data();
		this.processors = {};
		// Used to run processors.
		this.timeoutTimestamp = 0;
		this.timeoutProcessors = null;
	}

	/// Register a statistic provider.
	///
	/// \param provider The statistics provider.
	/// \param namespace The namespace of the statistics.
	register(provider, ...namespace) {
		provider.proxy.register(this.data, this.processors, ...namespace);
	}

	/// Create a provider and attach it to this server.
	makeProvider(...namespace) {
		const provider = new Provider(...namespace);
		this.register(provider);
		return provider;
	}

	installRest(api) {
		Log.info("Installing 'Statistics' REST.");

		const data = this.data;
		api.handle("get", "/admin/statistics", async function (context) {
			await handleDataGet(this, data, "statistics", []);
		});
	}

	/// Starting statistics.
	async start() {
		Exception.assert(this.timeoutProcessors === null, "Statistics is already started.");
		const processorWorkload = () => {
			const timestampStart = Date.now();
			for (const [provider, processor] of Object.values(this.processors)) {
				processor.process((Date.now() - this.timeoutTimestamp) / 1000, provider);
			}
			const durationMs = Date.now() - timestampStart;
			// Should not take more than 1% of the workload.
			this.timeoutTimestamp = Date.now();
			this.timeoutProcessors = setTimeout(processorWorkload, Math.max(durationMs * 100, 1000));
		};

		this.timeoutTimestamp = Date.now();
		this.timeoutProcessors = setTimeout(processorWorkload, 1000);
	}

	/// Stopping statistics.
	async stop() {
		clearTimeout(this.timeoutProcessors);
	}
}
