import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("logger", "memory");
const Log = LogFactory("logger", "memory");

export default class LoggerMemory {
	constructor(options = {}) {
		this.options = Object.assign(
			{
				/// Keep maximum 100 entries.
				maxEntries: 100,
				/// Store the entries for the last x errors.
				maxErrors: 3,
			},
			options,
		);

		this.buffer = [];
		this.errors = [];
		Log.addBackend("memory", (...args) => this.processor(...args), "debug");
	}

	processor(date, level, topics, message) {
		// Keep all new entries.
		while (this.buffer.length >= this.options.maxEntries) {
			this.buffer.shift();
		}
		this.buffer.push({ date, level, topics, message });

		// If this is an error.
		if (level == "error") {
			while (this.errors.length >= this.options.maxErrors) {
				this.errors.shift();
			}
			this.errors.push([...this.buffer]);
		}
	}

	async installRest(api) {
		Log.info("Installing 'Logger' REST");

		api.handle("get", "/admin/logger/memory", async (inputs, user) => {
			return {
				logs: this.buffer,
				errors: this.errors,
			};
		});
	}
}
