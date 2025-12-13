import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Executor from "#bzd/apps/job_executor/backend/executor.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/command.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker extends Executor {
	async initialize(args) {
		this.command = new Command(["docker", "run", "--rm", this.schema["docker"], ...args]);
	}

	async getInfo() {
		return this.command.getInfo();
	}
}
