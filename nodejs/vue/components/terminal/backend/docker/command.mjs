import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";
import http from "http";

const Exception = ExceptionFactory("terminal", "docker");
const Log = LogFactory("terminal", "docker");

export default class CommandDocker extends CommandBase {
	constructor(name, options) {
		super(options);
		this.name = name;
		this.result = null;
		this.client = null;
	}

	async _startDockerContainer(args) {
		Exception.assertPrecondition(Array.isArray(args), "Arguments must be an array.");
		Exception.assertPrecondition(args.length >= 1, "There must be at least 1 argument.");

		this.setStatus(Status.running);
		const result = await this.localCommandToOutput([
			"docker",
			"run",
			"--rm",
			"-it",
			"--name",
			this.name,
			"--detach",
			...args,
		]);
		await result.join();
	}

	async _monitorDockerContainer() {
		Exception.assertPrecondition(this.client === null, "Monitoring already started.");

		const req = http.request({
			socketPath: "/run/user/1000/docker.sock",
			path: "/containers/" + this.name + "/attach?stream=1&stdout=1&stderr=1",
			method: "POST",
			headers: {
				Connection: "Upgrade",
				Upgrade: "tcp",
			},
		});

		return new Promise((resolve, reject) => {
			req.on("upgrade", (res, socket, head) => {
				this.setStatus(Status.running);

				this.client = socket;
				this.client.setEncoding("utf8");
				this.client.on("error", (err) => {
					this.event.trigger("output", "Error: " + String(err));
					this.setStatus(Status.failed);
				});
				this.client.on("data", (data) => {
					this.event.trigger("output", data);
				});
				this.client.on("close", () => {
					this.setStatus(Status.completed);
				});
				resolve();
			});
			req.on("error", (err) => {
				this.event.trigger("output", "Error: " + String(err));
				reject(new Exception(err));
			});
			req.end();
		});
	}

	/// Monitor an existing container.
	async attach() {
		await this._monitorDockerContainer();
	}

	/// Start and monitor an existing container.
	async detach(args) {
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
