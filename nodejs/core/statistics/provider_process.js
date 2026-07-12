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

			// Save the current snapshot as the baseline for the next interval tick.
			this.set("cpu.eventloop", eluDiff.utilization);

			const memoryUsage = process.memoryUsage();
			this.size("memory.rss.used", memoryUsage.rss);
			this.size("memory.heap.total", memoryUsage.heapTotal);
			this.size("memory.heap.used", memoryUsage.heapUsed);

			// Note: This is a semi-private API, but universally used in the Node ecosystem.
			const activeHandles = process._getActiveHandles().length;
			this.set("health.handles", activeHandles);

			// Convert nanoseconds to seconds.
			this.set("health.eventloop.delay.max", this.histogram.max / 1e9);
			this.set("health.eventloop.delay.mean", this.histogram.mean / 1e9);
			this.histogram.reset();
		}, 1000);
	}

	stop() {
		clearInterval(this.interval);
		this.histogram.disable();
	}
}
