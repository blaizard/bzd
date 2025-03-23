import Source from "#bzd/apps/artifacts/plugins/nodes/sources/source.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "source", "nodes");

export default class SourceNodes extends Source {
	constructor(...args) {
		super(...args);

		Exception.assert("host" in this.options, "The 'host' attribute must be set.");

		let optionsClient = {
			expect: "json",
		};
		if (this.options.token) {
			optionsClient["query"] = {
				t: this.options.token,
			};
		}
		this.client = new this.components.HttpClientFactory(
			this.options.host + "/x/" + (this.options.volume || this.storageName),
			optionsClient,
		);
	}

	async fetchRecords(tick) {
		const result = await this.client.get("/@records", {
			query: {
				tick: tick,
			},
		});

		const timestampRemote = result.timestamp;
		const timestampLocal = this.getTimestampMs();
		const next = result.next;
		const end = result.end;

		Exception.assert(
			result.version === this.plugin.version,
			"Invalid version for remote record: {} vs {} (tick remote: {})",
			result.version,
			this.plugin.version,
			tick,
		);
		const updatedRecords = result.records.map((record) =>
			Nodes.recordFromDisk(record).map(([uid, key, value, timestamp]) => {
				return [uid, key, value, timestamp - timestampRemote + timestampLocal];
			}),
		);

		return [updatedRecords, next, end];
	}
}
