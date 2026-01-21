import { spawn } from "child_process";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Event from "#bzd/nodejs/core/event.mjs";

const Exception = ExceptionFactory("run");

export const Status = Object.freeze({
	idle: "idle",
	running: "running",
	failed: "failed",
	completed: "completed",
	cancelled: "cancelled",
});

class ExecuteResult {
	constructor(subprocess, { maxOutputSize }) {
		this.subprocess = subprocess;
		this.maxOutputSize = maxOutputSize;
		this.promise = null;
		this.returncode = null;

		this.output = [];
		this.outputSize = 0;
		this.status = Status.idle;
		this.event = new Event();
		this.event.on("status", (status) => {
			this.status = status;
		});
	}

	async join() {
		await this.promise;
	}

	_addToBuffer(isStdout, data) {
		if (data) {
			// Trigger output events.
			if (isStdout) {
				this.event.trigger("stdout", data);
			} else {
				this.event.trigger("stderr", data);
			}
			this.event.trigger("output", data);

			this.output.push([isStdout, data]);
			this.outputSize += data.length;
			while (this.outputSize > this.maxOutputSize) {
				const removed = this.output.shift();
				this.outputSize -= removed[1].length;
			}
		}
	}

	on(topic, handler) {
		Exception.assertPrecondition(
			["status", "stdout", "stderr", "output"].includes(topic),
			"Unsupported topic: " + topic,
		);
		this.event.on(topic, handler);
	}

	remove(topic, handler) {
		Exception.assertPrecondition(
			["status", "stdout", "stderr", "output"].includes(topic),
			"Unsupported topic: " + topic,
		);
		this.event.remove(topic, handler);
	}

	async kill() {
		const promise = new Promise((resolve) => {
			this.subprocess.on("exit", () => {
				resolve();
			});
		});
		this.subprocess.kill();
		return promise;
	}

	getStdout() {
		return this.output
			.filter((entry) => entry[0])
			.map((entry) => entry[1])
			.join("");
	}

	getStderr() {
		return this.output
			.filter((entry) => !entry[0])
			.map((entry) => entry[1])
			.join("");
	}

	getOutput() {
		return this.output.map((entry) => entry[1]).join("");
	}

	writeToStdin(data) {
		this.subprocess.stdin.write(data);
	}

	getReturnCode() {
		return this.returncode;
	}

	isSuccess() {
		return this.returncode === 0;
	}

	isFailure() {
		return this.returncode !== 0;
	}
}

export async function localCommand(cmds, { stdout = null, stderr = null, maxOutputSize = 1000000 } = {}) {
	Exception.assert(Array.isArray(cmds) && cmds.length > 0, "No command to run");

	const subprocess = spawn(cmds[0], cmds.slice(1), {
		stdio: ["pipe", "pipe", "pipe"],
	});
	const result = new ExecuteResult(subprocess, { maxOutputSize: maxOutputSize });

	return new Promise((resolve, reject) => {
		// Helper for the output handlers.
		if (stdout === null) {
			stdout = [];
		}
		if (stderr === null) {
			stderr = [];
		}
		if (stdout === true) {
			stdout = [process.stdout.write];
		}
		if (stderr === true) {
			stderr = [process.stderr.write];
		}
		if (typeof stdout === "function") {
			stdout = [stdout];
		}
		if (typeof stderr === "function") {
			stderr = [stderr];
		}
		Exception.assert(Array.isArray(stdout), "Invalid stdout handlers");
		Exception.assert(Array.isArray(stderr), "Invalid stderr handlers");
		if (maxOutputSize > 0) {
			stdout.push((data) => result._addToBuffer(true, data));
			stderr.push((data) => result._addToBuffer(false, data));
		}

		subprocess.stdout.setEncoding("utf8");
		subprocess.stdout.on("data", (data) => {
			stdout.forEach((handler) => handler(data));
		});
		subprocess.stderr.setEncoding("utf8");
		subprocess.stderr.on("data", (data) => {
			stderr.forEach((handler) => handler(data));
		});

		// Do not reject not to end up with an unhandled rejection.
		result.promise = new Promise((finalResolve, _) => {
			subprocess.on("close", (code) => {
				if (typeof code === "number") {
					result.returncode = code;
					if (code == 0) {
						result.event.trigger("status", Status.completed);
						finalResolve();
					} else {
						result.event.trigger("status", Status.failed);
						const message = "Process failed with error code: " + code;
						stderr.forEach((handler) => handler(message));
						finalResolve();
					}
				} else {
					result.event.trigger("status", Status.cancelled);
					stderr.forEach((handler) => handler("Cancelled"));
					finalResolve();
				}
			});
		});

		subprocess.on("spawn", () => {
			result.event.trigger("status", Status.running);
			resolve(result);
		});
		subprocess.on("error", (err) => {
			result.event.trigger("status", Status.failed);
			const message = "Process failed to start: " + String(err);
			stderr.forEach((handler) => handler(message));
			reject(new Exception(message));
		});
	});
}
