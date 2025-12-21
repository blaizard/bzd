import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { spawn } from "child_process";
import Status from "#bzd/nodejs/vue/components/terminal/backend/status.mjs";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";

const Exception = ExceptionFactory("terminal", "local");

export default class Command extends CommandBase {
	constructor(options) {
		super(options);
		this.process = null;
		this.status = Status.idle;
		this.timestampStart = null;
		this.timestampStop = null;
	}

	async detach(command) {
		Exception.assertPrecondition(Array.isArray(command), "Command must be an array.");
		Exception.assertPrecondition(command.length >= 1, "There must be at least 1 command.");

		this.timestampStart = Date.now();
		this.status = Status.running;
		this.process = spawn("nodejs/vue/components/terminal/backend/local/bin/terminal", command);

		await this.subprocessMonitor(this.process, {
			untilSpawn: true,
			updateStatus: (status) => {
				this.status = status;
				this.timestampStop = Date.now();
			},
		});
	}

	// Get the status of the command.
	getStatus() {
		return this.status;
	}

	// Get information about the command.
	async getInfo() {
		return {
			status: this.getStatus(),
			timestampStart: this.timestampStart,
			timestampStop: this.timestampStop,
		};
	}

	/// Write data to the terminal.
	write(data) {
		Exception.assert(this.process !== null, "The command is not running.");
		this.process.stdin.write(data);
	}

	/// Kill the command.
	async kill() {
		this.process.kill();
	}

	/// Install the command to be used with websockets.
	installWebsocket(context) {
		super.installWebsocket(context);
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
