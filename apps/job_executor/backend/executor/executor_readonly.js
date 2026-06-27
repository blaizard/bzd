import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Status from "#bzd/apps/job_executor/backend/status.js";

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
			case Status.running:
				info.status = Status.terminated;
				break;
		}
		return info;
	}

	static visitorArgs(_type, args) {
		return args;
	}
}
