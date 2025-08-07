import Command from "#bzd/nodejs/vue/components/terminal/backend/command.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import installCommandWebsocket from "#bzd/nodejs/vue/components/terminal/backend/websocket.mjs";

const Exception = ExceptionFactory("commands");
const Log = LogFactory("commands");

export default class Commands {
	constructor() {
		this.commands = {};
		this.uid = 0;
	}

	/// Generate the command.
	makeCliFromJob(schema, args) {
		if ("command" in schema) {
			return [schema["command"], ...args];
		}
		if ("docker" in schema) {
			return ["docker", "run", "--rm", schema["docker"], ...args];
		}
		Exception.unreachable("Undefined command for this job: {:j}", schema);
	}

	/// Create a new command.
	make(type, schema, args) {
		const uid = ++this.uid;
		const cliArgs = this.makeCliFromJob(schema, args);
		const command = {
			type: type,
			command: new Command(cliArgs),
			args: args,
		};
		this.commands[uid] = command;
		return uid;
	}

	get_(uid) {
		Exception.assert(uid in this.commands, "Undefined job id '{}'.", uid);
		return this.commands[uid];
	}

	/// Run a specific command.
	execute(uid) {
		this.get_(uid).command.execute();
	}

	/// Kill a specific command.
	kill(uid) {
		this.get_(uid).command.kill();
	}

	getInfo(uid) {
		const data = this.get_(uid);
		return Object.assign(
			{
				type: data.type,
				args: data.args,
			},
			data.command.getInfo(),
		);
	}

	getAllInfo() {
		let jobs = {};
		for (const uid of Object.keys(this.commands)) {
			jobs[uid] = this.getInfo(uid);
		}
		return jobs;
	}

	installCommandWebsocket(context, uid) {
		installCommandWebsocket(context, this.get_(uid).command);
	}
}
