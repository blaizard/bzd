import Provider from "#bzd/nodejs/core/statistics/provider.js";

export default class ProviderProcess extends Provider {
	constructor() {
		super("process");
		this.interval = null;
	}

	start() {
		let timeStart = performance.now();
		let cpuUsageStart = process.cpuUsage();
		this.interval = setInterval(() => {
			// Get the delta and reset.
			const timeEnd = performance.now();
			const cpuUsageDiff = process.cpuUsage(cpuUsageStart);
			cpuUsageStart = process.cpuUsage();

			// Calculate the CPU usage percentage.
			const timeDiff = timeEnd - timeStart;
			const value = (cpuUsageDiff.user + cpuUsageDiff.system) / (timeDiff * 1000);
			timeStart = timeEnd;

			this.set("cpu", value);
			this.size("memory", process.memoryUsage().rss);
		}, 1000);
	}

	stop() {
		clearInterval(this.interval);
	}
}
