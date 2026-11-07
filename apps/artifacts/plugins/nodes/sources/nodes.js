import Source from "#bzd/apps/artifacts/plugins/nodes/sources/source.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.js";
import { FileDatabase } from "#bzd/apps/artifacts/plugins/nodes/files.js";
import { timestampUs } from "#bzd/nodejs/utils/timestamp.js";

const Exception = ExceptionFactory("artifacts", "nodes", "source", "nodes");
const Log = LogFactory("artifacts", "nodes", "source", "nodes");

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

		const timeDelayLocalRemote = timestampUs() - result.timestamp;
		const next = result.next;
		const end = result.end;

		Exception.assert(
			result.version === this.plugin.version,
			"Invalid version for remote record: {} vs {} (tick remote: {})",
			result.version,
			this.plugin.version,
			tick,
		);
		const pointers = new Set();
		const updatedRecords = result.records.map((record) =>
			Nodes.recordFromDisk(record).map(([uid, data, timestamp, isAbsoluteTimestamp]) => {
				const value = data[1];
				if (FileDatabase.isPointer(value)) {
					pointers.add(value);
				}
				return [uid, data, isAbsoluteTimestamp ? timestamp : timestamp + timeDelayLocalRemote, isAbsoluteTimestamp];
			}),
		);

		// Mirror the files referenced by the records before they are inserted.
		if (this.plugin.files) {
			for (const pointer of pointers) {
				try {
					await this.plugin.files.syncFrom(this.client, pointer);
				} catch (e) {
					if (e.code === 404) {
						Log.warning("The file '{}' is not available on the remote: {}", pointer, e.message);
					} else {
						throw e;
					}
				}
			}
		}

		return [updatedRecords, next, end];
	}
}
