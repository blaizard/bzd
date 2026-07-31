import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.js";

const Exception = ExceptionFactory("backend", "executor-shell");
const Log = LogFactory("backend", "executor-shell");

export default class ExecutorShell {
	static type = "shell";

	constructor(uid, contextJob) {
		this.uid = uid;
		this.contextJob = contextJob;
		this.command = new Command();
	}

	static async isSupported() {
		return true;
	}

	/// Discover currently running processes. To be used to resume jobs after a restart.
	static async discover() {
		return {};
	}

	async execute(args, onTerminate) {
		this.command.onTerminate = onTerminate;
		const env = { ...process.env };
		// We do not want to propagate BZD_NODE_UID to the job as it is unrelated to this node.
		// While BZD_NODE_TOKEN is fine as it can be reused.
		delete env.BZD_NODE_UID;
		await this.command.detach(["--cwd", this.contextJob.getRootPath().asPosix(), "--", ...args], {
			env: env,
		});
	}

	async kill() {
		await this.command.kill();
	}

	async getInfo() {
		return this.command.getInfo();
	}

	static visitorArgs(type, args, schema) {
		switch (type) {
			case "File":
				return args.map((arg) => "/sandbox/" + arg.file);
			case "post":
				return [schema["command"], ...args];
		}
		return args;
	}

	installWebsocket(context) {
		this.command.installWebsocket(context);
	}
}
