import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Executor from "#bzd/apps/job_executor/backend/executor.mjs";
import CommandDocker from "#bzd/nodejs/vue/components/terminal/backend/docker/command.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker extends Executor {
	constructor(root, schema) {
		super(root, schema);
		this.dockerName = null;
	}

	/// Discover currently running processes. To be used to resume jobs after a restart.
	static async discover() {}

	async execute(uid, args) {
		this.dockerName = "bzd-job-executor-" + uid;
		this.command = new CommandDocker(this.dockerName);
		await this.command.detach(["-v", this.root + ":/sandbox", "--workdir", "/sandbox", this.schema["docker"], ...args]);
	}

	async kill() {
		await this.command.kill();
	}

	async getInfo() {
		return await this.command.getInfo();
	}

	visitorArgs(type, arg) {
		switch (type) {
			case "File":
				return "/sandbox/" + arg;
		}
		return arg;
	}

	installWebsocket(context) {
		this.command.installWebsocket(context);
	}
}
