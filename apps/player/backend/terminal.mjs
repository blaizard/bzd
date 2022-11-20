import pty from "node-pty";
import Event from "bzd/core/event.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";

const Exception = ExceptionFactory("terminal");

export default class Terminal {
	constructor(cwd) {
		this.config = {
			cols: 200,
			rows: 30,
			prompt: "\x1b[0;33mbzd\x1b[0m:\x1b[34m~/`basename " + cwd + "`\x1b[0m$ ",
		};
		this.cwd = cwd;
		this.event = new Event();
		this.process = null;
	}

	init(config) {
		// Process what needs to be changed.
		// Check if the dimension changed.
		let resize = false;
		if ("cols" in config && config.cols != this.condig.cols) {
			this.config.cols = config.cols;
			resize = true;
		}
		if ("rows" in config && config.rows != this.condig.rows) {
			this.config.rows = config.rows;
			resize = true;
		}
		let prompt = false;
		if ("prompt" in config && config.prompt != this.condig.prompt) {
			this.config.prompt = config.prompt;
			prompt = true;
		}

		// If the process is already initialized...
		if (this.process !== null) {
			// If only a resize is needed, use the resize function only.
			if (resize && !prompt) {
				this.process.resize(this.config.cols, this.config.rows);
				return;
			}
			// Otherwise, close this terminal.
			this.process.kill();
		}

		this.process = pty.spawn("/bin/bash", ["--noprofile", "--norc"], {
			name: "xterm-color",
			cols: this.config.cols,
			rows: this.config.rows,
			cwd: this.cwd,
			env: {
				PATH: "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
				PS1: this.config.prompt,
			},
		});
		this.process.on("data", (data) => {
			this.event.trigger("data", data);
		});
		this.process.on("exit", () => {
			this.event.trigger("exit");
		});
	}

	on(...args) {
		this.event.on(...args);
	}

	/// Write data to the terminal.
	write(data) {
		Exception.assert(this.process !== null, "The terminal is not initialized.");
		this.process.write(data);
	}
}