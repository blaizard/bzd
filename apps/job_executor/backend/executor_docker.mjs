import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Executor from "#bzd/apps/job_executor/backend/executor.mjs";
import CommandDocker from "#bzd/nodejs/vue/components/terminal/backend/docker/command.mjs";
import { localCommand } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker extends Executor {
	constructor(root) {
		super(root);
	}

	/// Discover currently running processes. To be used to resume jobs after a restart.
	static async discover() {
		const result = await localCommand(["docker", "ps", "--format", "json"]);
		await result.join();
	}

	async execute(uid, schema, args) {
		this.command = new CommandDocker("bzd-job-executor-" + uid);
		await this.command.detach(["-v", this.root + ":/sandbox", "--workdir", "/sandbox", schema["docker"], ...args]);
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
