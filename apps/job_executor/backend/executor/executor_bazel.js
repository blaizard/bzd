import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.js";
import ExecutorShell from "#bzd/apps/job_executor/backend/executor/executor_shell.js";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");

export default class ExecutorBazel extends ExecutorShell {
	static type = "bazel";

	async execute(args, onTerminate) {
		this.command.onTerminate = onTerminate;
		await this.command.detach(args);
	}

	static visitorArgs(type, args, schema) {
		switch (type) {
			case "File":
				return args.map((arg) => "/sandbox/" + arg.file);
			case "post":
				return [schema["target"], ...args];
		}
		return args;
	}
}
