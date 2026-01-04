import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import CommandBase from "#bzd/nodejs/vue/components/terminal/backend/base.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";
import http from "http";
import { localCommand } from "#bzd/nodejs/utils/run.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";

const Exception = ExceptionFactory("terminal", "docker");
const Log = LogFactory("terminal", "docker");

export default class CommandDocker extends CommandBase {
	constructor(name, options) {
		super(options);
		this.name = name;
		this.client = null;
		this.width = 120;
		this.height = 30;
	}

	/// Get the path of the docker socket.
	static #dockerSocketPath = null;
	static async getDockerSocket() {
		if (CommandDocker.#dockerSocketPath === null) {
			const result = await localCommand(["docker", "context", "inspect", "--format", "{{ .Endpoints.docker.Host }}"]);
			await result.join();
			const maybeSocket = result.getOutput().trim();
			if (maybeSocket) {
				Exception.assert(
					maybeSocket.startsWith("unix://"),
					"Docker socket must start with 'unix://', instead {}",
					maybeSocket,
				);
				CommandDocker.#dockerSocketPath = maybeSocket.replace("unix://", "");
			} else {
				CommandDocker.#dockerSocketPath = "/var/run/docker.sock";
			}
		}
		return CommandDocker.#dockerSocketPath;
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
			socketPath: await CommandDocker.getDockerSocket(),
			path: "/containers/" + this.name + "/attach?stream=1&stdout=1&stderr=1&stdin=1&tty=1",
			method: "POST",
			headers: {
				Connection: "Upgrade",
				Upgrade: "tcp",
			},
		});

		return new Promise((resolve, reject) => {
			req.on("upgrade", async (res, socket, head) => {
				this.setStatus(Status.running);

				this.client = socket;
				this.client.setEncoding("utf8");

				// Handle any data that might have arrived in the first packet.
				if (head && head.length > 0) {
					this.event.trigger("output", head.toString());
				}

				// Handle events.
				this.client.on("error", (err) => {
					this.event.trigger("output", "Error: " + String(err));
					this.setStatus(Status.failed);
				});
				this.client.on("data", (data) => {
					this.event.trigger("output", data);
				});
				this.client.on("close", () => {
					this.client = null;
					this.setStatus(Status.completed);
				});

				await this.resize(this.width, this.height);

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

	/// Resize the terminal.
	async resize(width, height) {
		this.width = width;
		this.height = height;
		if (!this.client) {
			return;
		}
		const host = await CommandDocker.getDockerSocket();
		await HttpClient.request("unix://" + host, {
			path: "/containers/" + this.name + "/resize",
			method: "POST",
			query: {
				w: width,
				h: height,
			},
		});
	}

	/// Write data to the terminal.
	write(data) {
		if (this.client && this.client.writable) {
			this.client.write(data);
		}
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
		this.installWebsocketForOutputAndInput(context);
	}
}
