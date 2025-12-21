import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Executor from "#bzd/apps/job_executor/backend/executor.mjs";
import { spawn } from "child_process";
import Event from "#bzd/nodejs/core/event.mjs";

const Exception = ExceptionFactory("backend", "executor-docker");
const Log = LogFactory("backend", "executor-docker");

export default class ExecutorDocker extends Executor {
	constructor(root, schema) {
		super(root, schema);
		this.output = "";
		this.dockerName = null;
		this.event = new Event();
		this.event.on("data", (data) => (this.output += data));
	}

	static async discover() {}

	async subprocessMonitor(subprocess) {
		return new Promise((resolve, reject) => {
			subprocess.stdout.setEncoding("utf8");
			subprocess.stdout.on("data", (data) => {
				this.event.trigger("data", data);
			});
			subprocess.stderr.setEncoding("utf8");
			subprocess.stderr.on("data", (data) => {
				this.event.trigger("data", data);
			});
			subprocess.on("error", (err) => {
				reject(new Exception("Process failed to start: " + String(err)));
			});
			subprocess.on("close", (code) => {
				if (code == 0) {
					resolve();
				} else {
					reject(new Exception("Process failed with error code: " + code));
				}
			});
		});
	}

	async startDockerContainer(args) {
		const subprocess = spawn("docker", [
			"run",
			"--rm",
			"--name",
			this.dockerName,
			"-v",
			this.root + ":/sandbox",
			"--workdir",
			"/sandbox",
			"-d",
			this.schema["docker"],
			...args,
		]);
		await this.subprocessMonitor(subprocess);
	}

	async monitorDockerContainer() {
		const subprocess = spawn("docker", ["logs", "--follow", this.dockerName]);
		this.subprocessMonitor(subprocess)
			.catch((err) => {
				Log.error("Docker container monitoring failed: {}", String(err));
			})
			.then(() => {
				this.dockerName = null;
			});
	}

	async execute(uid, args) {
		this.dockerName = "bzd-job-executor-" + uid;
		await this.startDockerContainer(args);
		await this.monitorDockerContainer();
	}

	async kill() {
		if (this.dockerName) {
			const subprocess = spawn("docker", ["kill", this.dockerName]);
			await this.subprocessMonitor(subprocess);
			this.dockerName = null;
		}
	}

	/// Install the command to be used with websockets.
	installWebsocket(context) {
		const onData = (data) => {
			context.send(data);
		};
		onData(this.output);
		this.event.on("data", onData);
		context.exit(() => {
			this.event.remove("data", onData);
		});
	}

	async getInfo() {
		return {}; //this.command.getInfo();
	}

	visitorArgs(type, arg) {
		switch (type) {
			case "File":
				return "/sandbox/" + arg;
		}
		return arg;
	}
}
