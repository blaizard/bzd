import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("terminal", "docker");
const Log = LogFactory("terminal", "docker");

export default class CommandDocker extends CommandBase {
	constructor(name, options) {
		super(options);
		this.name = name;
		this.result = null;
	}

	async _startDockerContainer(args) {
		Exception.assertPrecondition(Array.isArray(args), "Arguments must be an array.");
		Exception.assertPrecondition(args.length >= 1, "There must be at least 1 argument.");

		const result = await this.localCommandToOutput(["docker", "run", "--rm", "--name", this.name, "--detach", ...args]);
		await result.join();
	}

	async _monitorDockerContainer() {
		this.result = await this.localCommandToOutput(["docker", "logs", "--follow", "--tail", "1000", this.name]);
		this.result.on("status", (status) => {
			this.setStatus(status);
		});
	}

	/// Monitor an existing container.
	async attach() {
		this.setStatus(Status.running);
		await this._monitorDockerContainer();
	}

	/// Start and monitor an existing container.
	async detach(args) {
		this.setStatus(Status.running);
		await this._startDockerContainer(args);
		await this._monitorDockerContainer();
	}

	async kill() {
		try {
			const result = await this.localCommandToOutput(["docker", "kill", this.name]);
			await result.join();
		} catch (_) {
			// Ignore in case of error, kill should not throw if the container does not exists anymore.
		}
	}

	/// Install the command to be used with websockets.
	installWebsocket(context) {
		this.installWebsocketForOutput(context);
	}
}
