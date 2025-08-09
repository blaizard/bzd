import Event from "#bzd/nodejs/core/event.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { spawn } from "child_process";
import Status from "#bzd/nodejs/vue/components/terminal/backend/status.mjs";

const Exception = ExceptionFactory("terminal", "backend");

export default class Command {
	constructor(command, options) {
		Exception.assertPrecondition(Array.isArray(command), "Command must be an array.");
		Exception.assertPrecondition(command.length >= 1, "There must be at least 1 command.");
		this.options = Object.assign(
			{
				/// Maximum number of bytes in the stdout.
				maxOutputSize: 10000,
			},
			options,
		);
		this.command = command;
		this.event = new Event();
		this.process = null;
		this.output = [];
		this.outputSize = 0;
		this.status = Status.idle;
		this.timestampStart = null;
		this.timestampStop = null;
	}

	execute() {
		this.timestampStart = Date.now();
		this.status = Status.running;
		this.process = spawn("nodejs/vue/components/terminal/backend/bin/terminal", this.command, {
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
			if (typeof code === "number") {
				if (code !== 0) {
					this.status = Status.failed;
					this.addToOutput("Failed with exit code: " + code + ".");
				} else {
					this.status = Status.completed;
				}
			} else {
				this.status = Status.cancelled;
				this.addToOutput("Cancelled.");
			}
			this.event.trigger("exit");
		});
	}

	addToOutput(data) {
		if (data) {
			this.output.push(data);
			this.outputSize += data.length;
			while (this.outputSize > this.options.maxOutputSize) {
				const removed = this.output.shift();
				this.outputSize -= removed.length;
			}
		}
	}

	// Get the status of the command.
	getStatus() {
		return this.status;
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

	/// Kill the command.
	kill() {
		this.process.kill();
	}

	/// Install the command to be used with websockets.
	installWebsocket(context) {
		const onData = (data) => {
			context.send(data);
		};

		// Send data to the client.
		this.on("data", onData);
		context.exit(() => {
			this.remove("data", onData);
		});

		context.read((data) => {
			const input = JSON.parse(data.toString());
			switch (input.type) {
				case "init":
					// ignore init in this configuration.
					break;
				case "stream":
					this.write(input.value);
					break;
				default:
					Log.error("Unsupported data type '{}' for terminal.", input.type);
			}
		});
	}
}
