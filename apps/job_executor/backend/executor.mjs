import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");

export default class Executor {
	constructor(contextJob) {
		this.contextJob = contextJob;
	}

	async getInfo() {
		return {};
	}

	visitorArgs(_type, arg) {
		return arg;
	}

	installWebsocket(context) {
		const pathLog = this.contextJob.getLog().asPosix();
		FileSystem.readFile(pathLog)
			.then((data) => {
				context.send(data);
			})
			.catch(() => {});
	}
}
