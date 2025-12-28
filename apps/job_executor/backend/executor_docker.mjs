import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import CommandDocker from "#bzd/nodejs/vue/components/terminal/backend/docker/command.mjs";
import { localCommand } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker {
	static type = "docker";

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
		let executors = {};
		const regex = /^bzd-job-executor-(\d+)$/;
		for (const name of containers) {
			const match = name.match(regex);
			if (match) {
				const uid = parseInt(match[1], 10);
				const maybeContextJob = context.getJob(uid, null);
				executors[uid] = new ExecutorDocker(maybeContextJob);
				await executors[uid].attach(uid);
			}
		}
		return executors;
	}

	async attach(uid) {
		this.command = new CommandDocker("bzd-job-executor-" + uid);
		await this.command.attach();
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
		if (this.command) {
			await this.command.kill();
		}
	}

	async getInfo() {
		if (this.command) {
			return this.command.getInfo();
		}
		return {};
	}

	visitorArgs(type, arg, schema) {
		switch (type) {
			case "File":
				return "/sandbox/" + arg;
			case "post":
				return [schema["image"], ...arg];
		}
		return arg;
	}

	installWebsocket(context) {
		if (this.command) {
			this.command.installWebsocket(context);
		}
	}
}
