import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { spawn } from "child_process";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";

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
		const subprocess = spawn("docker", ["logs", "--follow", this.name]);
		await this.subprocessMonitor(subprocess, { untilSpawn: true });
	}

	async detach(args) {
		await this._startDockerContainer(args);
		await this._monitorDockerContainer();
	}

	async kill() {
		const subprocess = spawn("docker", ["kill", this.name]);
		await this.subprocessMonitor(subprocess);
	}

	async getInfo() {
		return {}; //this.command.getInfo();
	}
}
