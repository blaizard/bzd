import Provider from "#bzd/nodejs/core/statistics/provider.js";
import { performance, monitorEventLoopDelay } from "perf_hooks";

export default class ProviderProcess extends Provider {
	constructor() {
		super("process");
		this.interval = null;
		this.lastELU = null;
		this.histogram = null;
	}

	start() {
		this.histogram = monitorEventLoopDelay({ resolution: 1 });
		this.histogram.enable();
		this.lastELU = performance.eventLoopUtilization();
		this.interval = setInterval(() => {
			// Get the current absolute snapshot.
			const currentELU = performance.eventLoopUtilization();
			const eluDiff = performance.eventLoopUtilization(currentELU, this.lastELU);
			this.lastELU = currentELU;
			const memoryUsage = process.memoryUsage();

			this.set(["cpu", "eventloop"], [eluDiff.utilization], { unit: "%" })
				.size(["memory", "rss", "used"], memoryUsage.rss)
				.size(["memory", "heap", "total"], memoryUsage.heapTotal)
				.size(["memory", "heap", "used"], memoryUsage.heapUsed)
				.set("health.handles", process._getActiveHandles().length)
				.set("health.eventloop.delay.max", this.histogram.max / 1e9, { unit: "s" })
				.set("health.eventloop.delay.mean", this.histogram.mean / 1e9, { unit: "s" });

			this.histogram.reset();
		}, 1000);
	}

	stop() {
		clearInterval(this.interval);
		this.histogram.disable();
	}
}
