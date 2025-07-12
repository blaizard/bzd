import Event from "#bzd/nodejs/core/event.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { spawn } from "child_process";

const Exception = ExceptionFactory("terminal");

export default class Command {
	constructor(command, options) {
		Exception.assertPrecondition(Array.isArray(command), "Command must be an array.");
		Exception.assertPrecondition(command.length >= 1, "There must be at least 1 command.");
		this.options = Object.assign(
			{
				cwd: null,
			},
			options,
		);
		this.command = command;
		this.event = new Event();
		this.process = null;
		this.output = [];
		this.outputSize = 0;
	}

	start() {
		const [command, ...args] = this.command;
		this.process = spawn(command, args, {
			stdio: ["pipe", "pipe", "pipe"],
		});

		this.process.stdout.on("data", (data) => {
			this.addToOutput(data.toString());
			this.event.trigger("data", data.toString());
		});
		this.process.stderr.on("data", (data) => {
			this.addToOutput(data.toString());
			this.event.trigger("data", data.toString());
		});
		this.process.on("close", () => {
			this.event.trigger("exit");
			this.process = null;
		});
	}

	addToOutput(data) {
		this.output.push(data);
		this.outputSize += data.length;
		while (this.outputSize > 1000) {
			const removed = this.output.shift();
			this.outputSize -= removed.length;
		}
	}

	on(topic, callback) {
		switch (topic) {
			case "data":
				for (const data of this.output) {
					callback(data);
				}
				this.event.on("data", callback);
				break;
			case "exit":
				this.event.on("exit", callback);
				break;
			default:
				Exception.assertPrecondition(topic, "Subscription topic {} unsupported.", topic);
		}
	}

	/// Write data to the terminal.
	write(data) {
		Exception.assert(this.process !== null, "The command is not running.");
		this.process.stdin.write(data);
	}
}
