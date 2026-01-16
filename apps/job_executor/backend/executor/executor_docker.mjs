import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import CommandDocker from "#bzd/nodejs/vue/components/terminal/backend/docker/command.mjs";
import { localCommand } from "#bzd/nodejs/utils/run.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker {
	static type = "docker";

	constructor(uid, contextJob) {
		this.uid = uid;
		this.contextJob = contextJob;
		this.command = new CommandDocker("bzd-job-executor-" + this.uid);
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
				executors[uid] = new ExecutorDocker(uid, maybeContextJob);
				await executors[uid].attach();
			}
		}
		return executors;
	}

	async attach() {
		await this.command.attach();
	}

	async execute(args) {
		await this.command.detach([
			"-v",
			this.contextJob.getRootPath().absolute().asPosix() + ":/sandbox",
			//"--cpus", "1",
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

	static visitorArgs(type, args, schema) {
		switch (type) {
			case "File":
				return args.map((arg) => "/sandbox/" + arg.file);
			case "post":
				return [schema["image"], ...args];
		}
		return args;
	}

	installWebsocket(context) {
		this.command.installWebsocket(context);
	}
}
