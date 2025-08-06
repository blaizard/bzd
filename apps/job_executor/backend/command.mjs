import Event from "#bzd/nodejs/core/event.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { spawn } from "child_process";

const Exception = ExceptionFactory("backend", "command");

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
		this.timestampStart = null;
		this.timestampStop = null;
	}

	execute() {
		const [command, ...args] = this.command;
		this.timestampStart = Date.now();
		this.process = spawn(command, args, {
			stdio: ["pipe", "pipe", "pipe"],
		});

		this.process.stdout.on("data", (data) => {
			const dataStr = data.toString();
			this.addToOutput(dataStr);
			this.event.trigger("data", dataStr);
		});
		this.process.stderr.on("data", (data) => {
			const dataStr = data.toString();
			this.addToOutput(dataStr);
			this.event.trigger("data", dataStr);
		});
		this.process.on("close", (code) => {
			this.timestampStop = Date.now();
			if (code !== 0) {
				this.addToOutput("Failed with exit code: " + code);
			}
			this.event.trigger("exit");
		});
	}

	addToOutput(data) {
		if (data) {
			this.output.push(data);
			this.outputSize += data.length;
			while (this.outputSize > 1000) {
				const removed = this.output.shift();
				this.outputSize -= removed.length;
			}
		}
	}

	// Get the status of the command.
	getStatus() {
		if (this.process === null) {
			return "idle";
		}
		const exitCode = this.process.exitCode;
		if (exitCode === null) {
			return "running";
		}
		if (exitCode === 0) {
			return "completed";
		}
		return "failed";
	}

	// Get information about the command.
	getInfo() {
		return {
			status: this.getStatus(),
			timestampStart: this.timestampStart,
			timestampStop: this.timestampStop,
		};
	}

	// Subscribe from a topic.
	on(topic, callback) {
		switch (topic) {
			case "data":
				for (const data of this.output) {
					callback(data);
				}
				this.event.on("data", (data) => {
					callback(data);
				});
				break;
			case "exit":
				this.event.on("exit", (data) => {
					callback(data);
				});
				break;
			default:
				Exception.assertPrecondition(topic, "Subscription topic {} unsupported.", topic);
		}
	}

	// Unsubscribe from a topic.
	remove(topic, callback) {
		this.event.remove(topic, callback);
	}

	/// Write data to the terminal.
	write(data) {
		Exception.assert(this.process !== null, "The command is not running.");
		this.process.stdin.write(data);
	}
}
