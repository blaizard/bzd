import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { spawn } from "child_process";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";
import Status from "#bzd/nodejs/vue/components/terminal/backend/status.mjs";

const Exception = ExceptionFactory("terminal", "docker");
const Log = LogFactory("terminal", "docker");

export default class CommandDocker extends CommandBase {
	constructor(name, options) {
		super(options);
		this.name = name;
	}

	async _startDockerContainer(args) {
		Exception.assertPrecondition(Array.isArray(args), "Arguments must be an array.");
		Exception.assertPrecondition(args.length >= 1, "There must be at least 1 argument.");
		const subprocess = spawn("docker", ["run", "--rm", "--name", this.name, "--detach", ...args]);
		await this.subprocessMonitor(subprocess);
	}

	async _monitorDockerContainer() {
		const subprocess = spawn("docker", ["logs", "--follow", "--tail", "1000", this.name]);
		await this.subprocessMonitor(subprocess, {
			untilSpawn: true,
			updateStatus: (status) => {
				this.setStatus(status);
			},
		});
	}

	async detach(args) {
		this.setStatus(Status.running);
		await this._startDockerContainer(args);
		await this._monitorDockerContainer();
	}

	async kill() {
		const subprocess = spawn("docker", ["kill", this.name]);
		await this.subprocessMonitor(subprocess);
	}
}
