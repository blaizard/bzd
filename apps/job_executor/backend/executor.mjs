import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");

export default class Executor {
	constructor(root, schema) {
		this.root = root;
		this.schema = schema;
		this.command = null;
	}

	/// Discover currently running processes. To be used to resume jobs after a restart.
	static async discover() {}

	async execute(uid, args) {
		this.command = new Command(["--cwd", this.root, "--", this.schema["command"], ...args]);
		this.command.execute().catch((_) => {});
	}

	async kill() {
		this.command.kill();
	}

	async getInfo() {
		return this.command.getInfo();
	}

	visitorArgs(_type, arg) {
		return arg;
	}

	installWebsocket(context) {
		this.command.installWebsocket(context);
	}
}
