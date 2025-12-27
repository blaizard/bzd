import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");

export default class Executor {
	constructor(contextJob) {
		this.contextJob = contextJob;
		this.command = null;
	}

	/// Discover currently running processes. To be used to resume jobs after a restart.
	static async discover() {}

	async execute(uid, args) {
		this.command = new Command();
		await this.command.detach(["--cwd", this.contextJob.getRootPath().asPosix(), "--", ...args]);
	}

	async kill() {
		await this.command.kill();
	}

	async getInfo() {
		return this.command.getInfo();
	}

	visitorArgs(type, arg, schema) {
		switch (type) {
			case "post":
				return [schema["command"], ...arg];
		}
		return arg;
	}

	installWebsocket(context) {
		this.command.installWebsocket(context);
	}
}
