import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";

const Exception = ExceptionFactory("terminal", "local");

export default class Command extends CommandBase {
	constructor(options) {
		super(options);
		this.subprocess = null;
		this.result = null;
	}

	async detach(command) {
		Exception.assertPrecondition(Array.isArray(command), "Command must be an array.");
		Exception.assertPrecondition(command.length >= 1, "There must be at least 1 command.");

		this.setStatus(Status.running);
		this.result = await this.localCommandToOutput([
			"nodejs/vue/components/terminal/backend/local/bin/terminal",
			...command,
		]);
		this.result.on("status", (status) => {
			this.setStatus(status);
		});
	}

	/// Write data to the terminal.
	write(data) {
		Exception.assertPrecondition(this.result !== null, "The command is not running.");
		this.result.writeToStdin(data);
	}

	/// Kill the command.
	async kill() {
		this.result.kill();
	}

	/// Install the command to be used with websockets.
	installWebsocket(context) {
		this.installWebsocketForOutputAndInput(context);
	}
}
