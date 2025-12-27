import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import CommandDocker from "#bzd/nodejs/vue/components/terminal/backend/docker/command.mjs";
import { localCommand } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker {
	constructor(contextJob) {
		this.contextJob = contextJob;
		this.command = null;
	}

	/// Discover currently running processes. To be used to resume jobs after a restart.
	static async discover(context) {
		const result = await localCommand(["docker", "ps", "--format", "json"]);
		await result.join();
		const containers = result
			.getOutput()
			.split("\n")
			.map((s) => s.trim())
			.filter(Boolean)
			.map((line) => {
				const data = JSON.parse(line);
				return data["Names"];
			});
		let discovered = [];
		const regex = /^bzd-job-executor-(\d+)$/;
		for (const name of containers) {
			const match = name.match(regex);
			if (match) {
				const uid = parseInt(match[1], 10);
			}
		}
		console.log(containers);
	}

	async execute(uid, args) {
		this.command = new CommandDocker("bzd-job-executor-" + uid);
		await this.command.detach([
			"-v",
			this.contextJob.getRootPath().absolute().asPosix() + ":/sandbox",
			"--workdir",
			"/sandbox",
			...args,
		]);
	}

	async kill() {
		await this.command.kill();
	}

	async getInfo() {
		return this.command.getInfo();
	}

	visitorArgs(type, arg, schema) {
		switch (type) {
			case "File":
				return "/sandbox/" + arg;
			case "post":
				return [schema["docker"], ...arg];
		}
		return arg;
	}

	installWebsocket(context) {
		this.command.installWebsocket(context);
	}
}
