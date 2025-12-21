import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { spawn } from "child_process";
import Status from "#bzd/nodejs/vue/components/terminal/backend/status.mjs";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";

const Exception = ExceptionFactory("terminal", "local");

export default class Command extends CommandBase {
	constructor(options) {
		super(options);
		this.subprocess = null;
	}

	async detach(command) {
		Exception.assertPrecondition(Array.isArray(command), "Command must be an array.");
		Exception.assertPrecondition(command.length >= 1, "There must be at least 1 command.");

		this.setStatus(Status.running);
		this.subprocess = spawn("nodejs/vue/components/terminal/backend/local/bin/terminal", command);

		await this.subprocessMonitor(this.subprocess, {
			untilSpawn: true,
			updateStatus: (status) => {
				this.setStatus(status);
			},
		});
	}

	/// Write data to the terminal.
	write(data) {
		Exception.assert(this.subprocess !== null, "The command is not running.");
		this.subprocess.stdin.write(data);
	}

	/// Kill the command.
	async kill() {
		this.subprocess.kill();
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
