import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Status from "#bzd/apps/job_executor/backend/status.mjs";

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

	static visitorArgs(_type, arg) {
		return arg;
	}
}
