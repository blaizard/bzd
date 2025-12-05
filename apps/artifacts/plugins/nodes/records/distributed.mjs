import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records/records.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// Similar than Records but works with multiple host on the same disk.
///
/// They share a directory with the following structure:
/// - root
///    - hash1
///    - hash2
///    - hash3
///
/// Where the content of the host directory is the same as for Records.
export default class RecordsDistributed {
	/// Constructor for the distributed records.
	///
	/// \param hostname The hostname of the current machine.
	/// \param options for the records.
	constructor(options = {}) {
		this.options = Object.assign(
			{
				fs: FileSystem,
			},
			options,
		);
		this.records = null;
	}

	/// Return a random uid.
	_uid() {
		return parseInt(Date.now()) + "-" + Math.random().toString(36).substring(2, 12);
	}

	/// Initialize the distributed records.
	async init(servicesProvider, callback) {
		Exception.assertPrecondition(this.records === null, "Records are already initialized.");
		Exception.assertPrecondition(this.options.path, "Missing records path.");
		Exception.assertPrecondition(this.options.locks, "Missing locks.");

		let path = null;
		let lock = null;

		// Look if there are available records directory.
		await this.options.fs.mkdir(this.options.path, { force: true });
		const files = await this.options.fs.readdir(this.options.path, /*withFileTypes*/ true);
		for (const dirent of files) {
			if (dirent.isDirectory()) {
				const potentialPath = this.options.path + "/" + dirent.name;
				lock = await this.options.locks.tryLock(potentialPath);
				if (lock !== null) {
					path = potentialPath;
					Log.info("Acquiring distributed storage root '{}'.", path);
					break;
				}
			}
		}

		// If not create a new one.
		if (path === null) {
			path = this.options.path + "/" + this._uid();
			lock = await this.options.locks.tryLock(path);
			await this.options.fs.mkdir(path, { force: true });
			Log.info("Creating distributed storage root '{}'.", path);
		}

		Exception.assert(lock !== null, "Missing lock for distributed storage root: '{}'.", path);
		servicesProvider.addStopProcess("records.releasing", async () => {
			Log.info("Releasing distributed storage root '{}'.", path);
			await lock.unlock(/*force*/ true);
		});

		this.records = new Records(
			Object.assign({}, this.options, {
				path: path,
			}),
		);

		await this.records.init(callback);
	}

	async write(...args) {
		Exception.assertPrecondition(this.records !== null, "Records not initialized.");
		await this.records.write(...args);
	}

	async *read(tick = 0) {
		Exception.assertPrecondition(this.records !== null, "Records not initialized.");
		yield* this.records.read(tick);
	}

	getTickRemote(...args) {
		Exception.assertPrecondition(this.records !== null, "Records not initialized.");
		return this.records.getTickRemote(...args);
	}
}
