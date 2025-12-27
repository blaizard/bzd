import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");
export default class Executor {
	constructor(contextJob) {
		this.contextJob = contextJob;
	}

	async getInfo() {
		const info = await this.contextJob.getInfo();
		switch (info.status) {
			case Status.idle:
			case Status.running:
				info.status = "unknown";
				break;
		}
		return info;
	}

	visitorArgs(_type, arg) {
		return arg;
	}

	installWebsocket(context) {
		const pathLog = this.contextJob.getLogPath().asPosix();
		FileSystem.readFile(pathLog)
			.then((data) => {
				context.send(data);
			})
			.catch(() => {});
	}
}
