import Process from "#bzd/nodejs/core/services/process.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "source");

export default class Source extends Process {
	constructor(plugin, storageName, options, components) {
		super();
		this.plugin = plugin;
		this.storageName = storageName;
		this.options = Object.assign(
			{
				persistent: true,
			},
			options,
		);
		this.components = components;
		this.nextTickRemote = this.options.persistent ? this.plugin.records.getTickRemote(this.storageName, 0) : 0;

		Exception.assert(typeof this.fetchRecords === "function", "A source must implement a 'fetchRecords' function.");
	}

	async process(options) {
		try {
			const [records, next, end] = await this.fetchRecords(this.nextTickRemote);
			const currentTickRemote = this.nextTickRemote;
			this.nextTickRemote = next;

			// Apply the records from remote.
			for (const record of records) {
				await this.plugin.nodes.insertRecord(record);
				if (this.options.persistent) {
					await this.plugin.records.write(Nodes.recordToDisk(record), this.storageName, this.nextTickRemote);
				}
			}

			// Update the options.
			if (end === false) {
				options.periodS = 0.1;
			} else {
				const periodS = 600 * Math.exp(-records.length / 20);
				options.periodS = Math.min(periodS, 60);
			}

			return {
				tick: [currentTickRemote, this.nextTickRemote],
				end: end,
				nextCallS: options.periodS,
				records: records.length,
			};
		} catch (e) {
			options.periodS = 5 * 60; // Retry in 5min on error.
			throw e;
		}
	}
}
