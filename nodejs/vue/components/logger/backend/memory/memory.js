import LogFactory from "#bzd/nodejs/core/log.js";

const Log = LogFactory("logger", "memory");

export default class LoggerMemory {
	constructor(options = {}) {
		this.options = Object.assign(
			{
				/// Per-level capacity overrides.
				/// Any level not listed here uses `defaultCapacity`.
				capacities: {
					debug: 1000,
				},
				/// Capacity used for any level not present in `capacities`.
				defaultCapacity: 100,
			},
			options,
		);

		/// Dictionary of level -> ring buffer (Array).
		/// Buffers are created lazily on first encounter of each level.
		this.buffers = {};

		/// Register as a log backend at 'trace' level: receives all log entries (all levels including trace).
		Log.addBackend("memory", (...args) => this.processor(...args), "trace");
	}

	processor(date, level, topics, message) {
		if (!(level in this.buffers)) {
			this.buffers[level] = [];
		}
		const buffer = this.buffers[level];
		const capacity = this.options.capacities[level] ?? this.options.defaultCapacity;
		if (buffer.length >= capacity) {
			buffer.shift();
		}
		buffer.push({ date, level, topics, message });
	}

	installRest(api) {
		Log.info("Installing 'Logger' REST");

		api.handle("get", "/admin/logger", async (inputs, user) => {
			/// Return the raw per-level buffers. The frontend handles merge and sort.
			return { logs: this.buffers };
		});
	}
}
