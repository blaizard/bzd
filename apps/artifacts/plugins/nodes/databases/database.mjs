import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "database");
const Log = LogFactory("artifacts", "nodes", "database");

export default class Database {
	constructor(plugin, options, components) {
		this.plugin = plugin;
		this.options = options;
		this.components = components;
		this.tick = 0;

		Exception.assert(typeof this.onRecords === "function", "A database must implement a 'onRecords' function.");
		Exception.assert(typeof this.onExternal === "function", "A database must implement a 'onExternal' function.");
	}

	installServices(provider) {
		if (this.options.write) {
			provider.addTimeTriggeredProcess(
				"write",
				async (options) => {
					return await this.processTimeTriggered(options);
				},
				{
					policy: this.options.throwOnFailure ? Services.Policy.throw : Services.Policy.ignore,
					periodS: 5,
					delayS: this.options.delayS || null,
				},
			);
			provider.addStopProcess("write.remainings", async () => {
				Log.info("Writing remainings records to database.");
				let endOfRecords = false;
				while (!endOfRecords) {
					const result = await this.processNext();
					endOfRecords = result.end;
					Log.info("Wrote {} records, end={}.", result.records, endOfRecords);
				}
			});
		}
	}

	/// Write the next record to the database.
	async processNext() {
		const tickStart = this.tick;
		const result = await this.plugin.read(tickStart, this.options.maxSize || 1024 * 1024, /*diskFormat*/ false);

		const records = result.records.reduce((all, record) => all.concat(record), []);
		const output = await this.onRecords(records);

		// Set the tick after the onRecords callback is called to make sure that on errors we do not
		// skip the tick and will retry later on.
		this.tick = result.next;
		const end = result.end;

		return Object.assign(
			{
				tick: [tickStart, this.tick],
				end: end,
				records: result.records.length,
			},
			output,
		);
	}

	/// Wrapper around processNext to handle the recurrency period.
	///
	/// \param options The options from the time triggered process
	async processTimeTriggered(options) {
		try {
			const result = await this.processNext();

			// Update the options.
			if (result.end === false) {
				options.periodS = 0.1;
			} else {
				const periodS = 600 * Math.exp(-result.records / 20);
				options.periodS = Math.min(periodS, 60);
			}

			return Object.assign(
				{
					nextCallS: options.periodS,
				},
				result,
			);
		} catch (e) {
			options.periodS = 5 * 60; // Retry in 5min on error.
			throw e;
		}
	}
}
