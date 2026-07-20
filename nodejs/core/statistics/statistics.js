import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Provider from "#bzd/nodejs/core/statistics/provider.js";
import Data from "#bzd/nodejs/db/data/data.js";
import ServiceProvider from "#bzd/nodejs/core/services/provider.js";
import Services from "#bzd/nodejs/core/services/services.js";
import { timestampMs } from "#bzd/nodejs/utils/timestamp.js";
import { handleDataGet, getDataGetInputsFromQuery } from "#bzd/nodejs/db/data/backend/handler.js";
import { Node } from "#bzd/apps/artifacts/api/nodejs/node/node.js";

const Exception = ExceptionFactory("statistics");
const Log = LogFactory("statistics");

/// This class is a statistics manager.
export default class Statistics {
	constructor(options) {
		this.options = Object.assign(
			{
				// The uid for this node, this will be used to send statistics data with this uid name.
				// If unset, no data will be sent.
				uid: null,
			},
			options,
		);

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
			const inputs = getDataGetInputsFromQuery(this);
			const maybeOutput = await handleDataGet(
				data,
				Object.assign(
					{
						uid: "statistics",
						key: [],
					},
					inputs,
				),
			);

			if (maybeOutput === null) {
				throw this.httpError(404, "Not found");
			}
			return maybeOutput;
		});
	}

	serviceSync(...namespaces) {
		const provider = new ServiceProvider(...namespaces);

		if (this.options.uid) {
			Log.info("Installing 'Statistics' services with uid={}.", this.options.uid);

			let lastTimestamp = 0;
			const node = new Node({ uid: this.options.uid });
			provider.addTimeTriggeredProcess(
				"statistics.sync",
				async (options) => {
					const maybeData = await this.data.get({
						uid: "statistics",
						key: [],
						metadata: true,
						children: 99,
						after: lastTimestamp,
					});
					lastTimestamp = timestampMs();
					if (maybeData.isEmpty()) {
						return "empty";
					}
					await node.publishBulk({}, (publish) => {
						for (const [key, [timestampMs, value, expires, unit]] of maybeData.value()) {
							publish({ key: key, timestampMs: timestampMs, value: value, unit: unit });
						}
					});
				},
				{
					policy: Services.Policy.ignore,
					periodS: 5,
				},
			);
		}
		return provider;
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
