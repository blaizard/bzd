import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records/records.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// Similar than Records but works with multiple host on the same disk.
///
/// They share a directory with the following structure:
/// - root
///    - current_host
///    - host1
///    - host2
///
/// Where the content of the host directory is the same as for Records.
export default class RecordsDistributed {
	/// Constructor for the distributed records.
	///
	/// \param hostname The hostname of the current machine.
	/// \param options for the records.
	constructor(hostname, options = {}) {
		this.hostname = hostname;
		this.options = options;
		this.records = new Records(
			Object.assign({}, this.options, {
				path: this.options.path + "/" + this.hostname,
			}),
		);
	}

	/// Initialize the distributed records.
	async init(servicesProvider, callback) {
		await this.records.init(callback);
	}

	async write(...args) {
		await this.records.write(...args);
	}

	async *read(tick = 0) {
		yield* this.records.read(tick);
	}

	getTickRemote(...args) {
		return this.records.getTickRemote(...args);
	}
}
