import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";

import pathlib from "#bzd/nodejs/utils/pathlib.mjs";

const Exception = ExceptionFactory("backend", "context");
const Log = LogFactory("backend", "context");

class ContextJob {
	constructor(root, storage, uid) {
		this.root_ = root;
		this.storage_ = storage;
		this.uid_ = uid;
	}

	/// Get the absolute root directory for this job.
	getRoot() {
		return this.root_.joinPath(this.getPrefix());
	}

	/// Get the prefix for this job.
	getPrefix() {
		return "job-" + this.uid_;
	}

	/// Initialize the current job context.
	async initialize() {
		await this.storage_.mkdir([this.getPrefix()]);
	}

	/// Copy the content of the path into this job context.
	///
	/// \param source The source path, should be an absolute path.
	/// \param destination The destination path, relative to the job root.
	async moveTo(source, destination) {
		const path = this.getRoot().joinPath(destination);
		await FileSystem.mkdir(path.parent.asPosix(), { force: true });
		await FileSystem.move(source, path.asPosix());
	}

	/// Destroy the current job context.
	async destroy() {
		await this.storage_.delete([this.getPrefix()]);
	}

	async metadata(pathList) {
		return await this.storage_.metadata([this.getPrefix(), ...pathList]);
	}

	async read(pathList) {
		return await this.storage_.read([this.getPrefix(), ...pathList]);
	}

	async list(pathList, maxOrPaging, includeMetadata) {
		return await this.storage_.list([this.getPrefix(), ...pathList], maxOrPaging, includeMetadata);
	}
}

/// Context class for job executor backend.
export default class Context {
	constructor(root) {
		this.root = pathlib.path(root);
		this.storage = null;
		// Current jobs contexts.
		this.jobs = {};
		this.uid = 0;
	}

	/// Allocate a new job context ID.
	allocate() {
		// Loop until we find an UID not used.
		while (++this.uid in this.jobs) {}
		return this.uid;
	}

	getAllJobs() {
		return this.jobs;
	}

	async initialize() {
		this.storage = await StorageDisk.make(this.root.asPosix(), {
			mustExists: false,
			write: true,
		});
		const all = await this.storage.listAll([], 100, /*includeMetadata*/ true);
		const directories = all.filter((entry) => Permissions.makeFromEntry(entry).isList()).map((entry) => entry.name);
		const regex = /^job-(\d+)$/;
		for (const name of directories) {
			const match = name.match(regex);
			if (match) {
				const uid = parseInt(match[1], 10);
				this.jobs[uid] = new ContextJob(this.root, this.storage, uid);
				Log.info("Discovered previous job context '{}'.", uid);
			} else {
				Log.warning("Unexpected directory '{}'.", name);
			}
		}
	}

	async addJob(uid) {
		Exception.assertPrecondition(!(uid in this.jobs), "The uid '{}' has already a job context associated to it.", uid);
		this.jobs[uid] = new ContextJob(this.root, this.storage, uid);
		await this.jobs[uid].initialize();
		return this.jobs[uid];
	}

	getJob(uid) {
		Exception.assertPrecondition(uid in this.jobs, "There is not uid '{}' associated with a job context.", uid);
		return this.jobs[uid];
	}

	async removeJob(uid) {
		Exception.assertPrecondition(uid in this.jobs, "There is not uid '{}' associated with a job context.", uid);
		await this.jobs[uid].destroy();
		delete this.jobs[uid];
	}
}
