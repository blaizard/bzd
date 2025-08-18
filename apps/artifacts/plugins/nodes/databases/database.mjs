import Process from "#bzd/nodejs/core/services/process.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "database");

export default class Database extends Process {
	constructor(plugin, options, components) {
		super();
		this.plugin = plugin;
		this.options = options;
		this.components = components;
		this.tick = 0;

		Exception.assert(typeof this.onRecords === "function", "A database must implement a 'onRecords' function.");
		Exception.assert(typeof this.onExternal === "function", "A database must implement a 'onExternal' function.");
	}

	async process(options) {
		try {
			const tickStart = this.tick;
			const result = await this.plugin.read(tickStart, this.options.maxSize || 1024 * 1024, /*diskFormat*/ false);

			const records = result.records.reduce((all, record) => all.concat(record), []);
			const output = await this.onRecords(records);

			// Set the tick after the onRecords callback is called to make sure that on errors we do not
			// skip the tick and will retry later on.
			this.tick = result.next;
			const end = result.end;

			// Update the options.
			if (end === false) {
				options.periodS = 0.1;
			} else {
				const periodS = 600 * Math.exp(-result.records.length / 20);
				options.periodS = Math.min(periodS, 60);
			}

			return Object.assign(
				{
					tick: [tickStart, this.tick],
					end: end,
					nextCallS: options.periodS,
					records: result.records.length,
				},
				output,
			);
		} catch (e) {
			options.periodS = 5 * 60; // Retry in 5min on error.
			throw e;
		}
	}
}
