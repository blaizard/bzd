import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";

import pathlib from "#bzd/nodejs/utils/pathlib.mjs";

const Exception = ExceptionFactory("backend", "context");
const Log = LogFactory("backend", "context");

class JobLogCapture {
	constructor(path) {
		this.path = path;
	}
	send(data) {
		FileSystem.appendFile(this.path, data);
	}
	read(onRead) {}
	exit(onExit) {}
}

class ContextJob {
	constructor(context, uid) {
		this.context_ = context;
		this.uid_ = uid;
		this.prefix_ = "job-" + this.uid_;
		this.info = null;
	}

	/// Get the unique identifier for this job.
	getUid() {
		return this.uid_;
	}

	/// Get the absolute root directory for this job.
	getRootPath() {
		return this.context_.root.joinPath(...this.getPrefixData());
	}

	getLogPath() {
		return this.context_.root.joinPath(this.prefix_, "log.txt");
	}

	getInfoPath() {
		return this.context_.root.joinPath(this.prefix_, "info.json");
	}

	async getLogs(onLog) {
		try {
			const data = await FileSystem.readFile(this.getLogPath().asPosix());
			for (const line of data.split("\n")) {
				onLog(line + "\n");
			}
		} catch (e) {
			// ignore.
		}
	}

	/// Update the current information from this job.
	async updateInfo(info) {
		if (info) {
			const previousInfo = await this.getInfo();
			const updatedInfo = Object.assign({}, previousInfo, info);
			const updatedInfoSerialized = JSON.stringify(updatedInfo);
			if (updatedInfoSerialized != this.info[1]) {
				await FileSystem.writeFile(this.getInfoPath().asPosix(), updatedInfoSerialized);
				this.info = [updatedInfo, updatedInfoSerialized];
			}
		}
		return this.info[0];
	}

	/// Get the information from this job.
	async getInfo() {
		if (this.info === null) {
			try {
				const raw = await FileSystem.readFile(this.getInfoPath().asPosix());
				this.info = [JSON.parse(raw), raw];
			} catch (e) {
				this.info = [{}, "{}"];
			}
		}
		return this.info[0];
	}

	/// Get the prefix for the data.
	getPrefixData() {
		return [this.prefix_, "sandbox"];
	}

	/// Initialize the current job context.
	async initialize() {
		await this.context_.storage.mkdir(this.getPrefixData());
	}

	/// Copy the content of the path into this job context.
	///
	/// \param source The source path, should be an absolute path.
	/// \param destination The destination path, relative to the job root.
	async moveTo(source, destination) {
		const path = this.getRootPath().joinPath(destination);
		await FileSystem.mkdir(path.parent.asPosix(), { force: true });
		await FileSystem.move(source, path.asPosix());
	}

	/// Capture the output of the given executor.
	captureOutput(executor) {
		if (executor.installWebsocket) {
			const context = new JobLogCapture(this.getLogPath().asPosix());
			executor.installWebsocket(context);
		}
	}

	/// Destroy the current job context.
	async destroy() {
		await this.context_.storage.delete([this.prefix_]);
		delete this.context_.jobs[this.uid_];
	}

	async metadata(pathList) {
		return await this.context_.storage.metadata([...this.getPrefixData(), ...pathList]);
	}

	async read(pathList) {
		return await this.context_.storage.read([...this.getPrefixData(), ...pathList]);
	}

	async list(pathList, maxOrPaging, includeMetadata) {
		return await this.context_.storage.list([...this.getPrefixData(), ...pathList], maxOrPaging, includeMetadata);
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
		return Object.keys(this.jobs);
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
				this.jobs[uid] = new ContextJob(this, uid);
				this.uid = Math.max(this.uid, uid);
				Log.info("Discovered previous job context '{}'.", uid);
			} else {
				Log.warning("Unexpected directory '{}'.", name);
			}
		}
	}

	async addJob(uid) {
		Exception.assertPrecondition(!(uid in this.jobs), "The uid '{}' has already a job context associated to it.", uid);
		this.jobs[uid] = new ContextJob(this, uid);
		await this.jobs[uid].initialize();
		return this.jobs[uid];
	}

	getJob(uid, valueOr = undefined) {
		if (valueOr === undefined) {
			Exception.assertPrecondition(uid in this.jobs, "There is not uid '{}' associated with a job context.", uid);
		}
		return this.jobs[uid] || valueOr;
	}
}
