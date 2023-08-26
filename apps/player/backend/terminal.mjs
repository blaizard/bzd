import Event from "#bzd/nodejs/core/event.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import { spawn } from "child_process";
import Path from "path";

const Exception = ExceptionFactory("terminal");

export default class Terminal {
	constructor(cwd, historyPath) {
		const dirname = Path.dirname(cwd);
		this.config = {
			cols: 1024,
			rows: 30,
			prompt: "\x1b[0;33mbzd\x1b[0m:\x1b[34m~${PWD:" + dirname.length + "}\x1b[0m$ ",
		};
		this.cwd = cwd;
		this.historyPath = historyPath;
		this.event = new Event();
		this.process = null;
	}

	async init(config) {
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

		// Make sure the cwd exists.
		await FileSystem.mkdir(this.cwd);

		this.process = spawn(
			"apps/player/backend/terminal",
			[
				"--cwd",
				this.cwd,
				"--env",
				"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
				"--env",
				"PS1=" + this.config.prompt,
				"--env",
				"HOME=" + process.env.HOME,
				"--",
				"--noprofile",
				"--norc",
			],
			{
				stdio: ["pipe", "pipe", "pipe"],
			}
		);

		//
		// this.process = pty.spawn("/bin/bash", ["--noprofile", "--norc"], {
		// name: "xterm-color",
		// cols: this.config.cols,
		// rows: this.config.rows,
		// cwd: this.cwd,
		// env: {
		// PATH: "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
		// PS1: this.config.prompt,
		// HOME: process.env.HOME,
		// },
		// });
		//
		this.process.stdout.on("data", (data) => {
			this.event.trigger("data", data.toString());
		});
		this.process.stderr.on("data", (data) => {
			this.event.trigger("data", data.toString());
		});
		this.process.on("close", () => {
			this.event.trigger("exit");
		});

		//
		// const path = Path.join(this.historyPath, "terminal-0.txt");
		// if (await FileSystem.exists(path)) {
		// const content = await FileSystem.readFile(path);
		// this.event.trigger("data", content);
		// }
		//
	}

	on(...args) {
		this.event.on(...args);
	}

	/// Write data to the terminal.
	async write(data) {
		Exception.assert(this.process !== null, "The terminal is not initialized.");
		this.process.stdin.write(data);
	}
}
