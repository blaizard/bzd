import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Event from "#bzd/nodejs/core/event.mjs";
import Status from "#bzd/nodejs/vue/components/terminal/backend/status.mjs";

const Exception = ExceptionFactory("terminal");
const Log = LogFactory("terminal");

export default class CommandBase {
	constructor(options) {
		this.options = Object.assign(
			{
				/// Maximum number of bytes in the stdout.
				maxOutputSize: 10000,
			},
			options,
		);
		this.output = [];
		this.outputSize = 0;
		this.status = Status.idle;
		this.timestampStart = null;
		this.timestampStop = null;

		this.event = new Event();
		this.event.on("data", (data) => {
			this.output.push(data);
			this.outputSize += data.length;
			while (this.outputSize > this.options.maxOutputSize) {
				const removed = this.output.shift();
				this.outputSize -= removed.length;
			}
		});
	}

	/// Get the status of the command.
	getStatus() {
		return this.status;
	}

	/// Set the status of the command.
	setStatus(status) {
		switch (status) {
			case Status.idle:
				break;
			case Status.running:
				this.timestampStart = Date.now();
				break;
			case Status.completed:
			case Status.failed:
			case Status.cancelled:
				this.timestampStop = Date.now();
				break;
			default:
				Exception.assertUnreachable(`Unsupported status value: ${status}`);
		}
		this.status = status;
	}

	/// Get information about the command.
	getInfo() {
		return {
			status: this.getStatus(),
			timestampStart: this.timestampStart,
			timestampStop: this.timestampStop,
		};
	}

	async subprocessMonitor(subprocess, { untilSpawn = false, updateStatus = (_) => {} } = {}) {
		return new Promise((resolve, reject) => {
			subprocess.stdout.setEncoding("utf8");
			subprocess.stdout.on("data", (data) => {
				this.event.trigger("data", data);
			});
			subprocess.stderr.setEncoding("utf8");
			subprocess.stderr.on("data", (data) => {
				this.event.trigger("data", data);
			});
			subprocess.on("spawn", () => {
				if (untilSpawn) {
					resolve();
					resolve = () => {};
					reject = (_) => {};
				}
			});
			subprocess.on("error", (err) => {
				updateStatus(Status.failed);
				const message = "Process failed to start: " + String(err);
				this.event.trigger("data", message);
				reject(new Exception(message));
			});
			subprocess.on("close", (code) => {
				if (typeof code === "number") {
					if (code == 0) {
						updateStatus(Status.completed);
						resolve();
					} else {
						updateStatus(Status.failed);
						const message = "Process failed with error code: " + code;
						this.event.trigger("data", message);
						reject(message);
					}
				} else {
					updateStatus(Status.cancelled);
					this.event.trigger("data", "Cancelled");
					resolve("Cancelled");
				}
			});
		});
	}

	/// Install the command to be used with websockets.
	installWebsocket(context) {
		const onData = (data) => {
			context.send(data);
		};
		for (const data of this.output) {
			onData(data);
		}
		this.event.on("data", onData);
		context.exit(() => {
			this.event.remove("data", onData);
		});
	}
}
