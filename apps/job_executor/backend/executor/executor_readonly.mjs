import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");
export default class ExecutorReadonly {
	constructor(uid, maybeContextJob) {
		Exception.assert(maybeContextJob, "Executor can only works with a valid ContextJob.");
		this.contextJob = maybeContextJob;
	}

	static async discover() {
		return {};
	}

	async getInfo() {
		const info = await this.contextJob.getInfo();
		switch (info.status) {
			case Status.idle:
			case Status.running:
				info.status = undefined;
				break;
		}
		return info;
	}

	visitorArgs(_type, arg) {
		return arg;
	}
}
