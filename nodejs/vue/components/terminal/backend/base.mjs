import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Event from "#bzd/nodejs/core/event.mjs";
import { localCommand, Status } from "#bzd/nodejs/utils/run.mjs";

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
		this.event.on("output", (data) => {
			this.output.push(data);
			this.outputSize += data.length;
			while (this.outputSize > this.options.maxOutputSize) {
				const removed = this.output.shift();
				this.outputSize -= removed.length;
			}
		});
	}

	async localCommandToOutput(cmds) {
		const result = await localCommand(cmds, {
			maxOutputSize: 0,
			stdout: (data) => {
				this.event.trigger("output", data);
			},
			stderr: (data) => {
				this.event.trigger("output", data);
			},
		});
		return result;
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

	/// Install the command to be used with websockets.
	installWebsocketForOutput(context) {
		const onOutput = (data) => {
			context.send(data);
		};
		for (const data of this.output) {
			onOutput(data);
		}
		this.event.on("output", onOutput);
		context.exit(() => {
			this.event.remove("output", onOutput);
		});
	}

	/// Install the command to be used with websockets.
	installWebsocketForOutputAndInput(context) {
		this.installWebsocketForOutput(context);
		context.read((data) => {
			const input = JSON.parse(data.toString());
			switch (input.type) {
				case "init":
					// ignore init in this configuration.
					break;
				case "stream":
					this.write(input.value);
					break;
				default:
					Log.error("Unsupported data type '{}' for terminal.", input.type);
			}
		});
	}
}
