import { spawn, type ChildProcess, type SpawnOptions } from "child_process";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Event from "#bzd/nodejs/core/event.js";

const Exception = ExceptionFactory("run");

export const Status = Object.freeze({
	idle: "idle",
	running: "running",
	failed: "failed",
	completed: "completed",
	cancelled: "cancelled",
});

type StatusType = (typeof Status)[keyof typeof Status];

type OutputHandler = (data: string) => void;

interface LocalCommandOptions {
	stdout?: boolean | OutputHandler | OutputHandler[] | null;
	stderr?: boolean | OutputHandler | OutputHandler[] | null;
	maxOutputSize?: number;
	ignoreFailure?: boolean;
	env?: NodeJS.ProcessEnv | null;
}

class ExecuteResult {
	subprocess: ChildProcess;
	maxOutputSize: number;
	promise: Promise<unknown> | null = null;
	returncode: number | null = null;

	output: [boolean, string][] = [];
	outputSize: number = 0;
	status: StatusType = Status.idle;
	event: Event = new Event();

	constructor(subprocess: ChildProcess, { maxOutputSize }: { maxOutputSize: number }) {
		this.subprocess = subprocess;
		this.maxOutputSize = maxOutputSize;
		this.event.on("status", (status: StatusType) => {
			this.status = status;
		});
	}

	async join(): Promise<ExecuteResult> {
		await this.promise;
		return this;
	}

	_addToBuffer(isStdout: boolean, data: string): void {
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
				const removed = this.output.shift()!;
				this.outputSize -= removed[1].length;
			}
		}
	}

	on(topic: string, handler: (...args: unknown[]) => void): void {
		Exception.assertPrecondition(
			["status", "stdout", "stderr", "output"].includes(topic),
			"Unsupported topic: " + topic,
		);
		this.event.on(topic, handler);
	}

	remove(topic: string, handler: (...args: unknown[]) => void): void {
		Exception.assertPrecondition(
			["status", "stdout", "stderr", "output"].includes(topic),
			"Unsupported topic: " + topic,
		);
		this.event.remove(topic, handler);
	}

	async kill(): Promise<void> {
		const promise = new Promise<void>((resolve) => {
			this.subprocess.on("exit", () => {
				resolve();
			});
		});
		this.subprocess.kill();
		return promise;
	}

	getStdout(): string {
		return this.output
			.filter((entry) => entry[0])
			.map((entry) => entry[1])
			.join("");
	}

	getStderr(): string {
		return this.output
			.filter((entry) => !entry[0])
			.map((entry) => entry[1])
			.join("");
	}

	getOutput(): string {
		return this.output.map((entry) => entry[1]).join("");
	}

	writeToStdin(data: string): void {
		this.subprocess.stdin!.write(data);
	}

	getReturnCode(): number | null {
		return this.returncode;
	}

	isSuccess(): boolean {
		return this.returncode === 0;
	}

	isFailure(): boolean {
		return this.returncode !== 0;
	}
}

export function localCommand(
	cmds: string[],
	{
		stdout = null,
		stderr = null,
		maxOutputSize = 1000000,
		ignoreFailure = false,
		env = null,
	}: LocalCommandOptions = {},
): ExecuteResult {
	Exception.assert(Array.isArray(cmds) && cmds.length > 0, "No command to run");

	const spawnOptions: SpawnOptions = {
		stdio: ["pipe", "pipe", "pipe"],
	};
	if (env !== null) {
		spawnOptions.env = env;
	}

	const subprocess = spawn(cmds[0], cmds.slice(1), spawnOptions);
	const result = new ExecuteResult(subprocess, { maxOutputSize: maxOutputSize });

	// Helper for the output handlers.
	let stdoutHandlers: OutputHandler[] = [];
	let stderrHandlers: OutputHandler[] = [];
	if (stdout === true) {
		stdoutHandlers = [process.stdout.write];
	} else if (typeof stdout === "function") {
		stdoutHandlers = [stdout];
	} else if (Array.isArray(stdout)) {
		stdoutHandlers = stdout;
	}
	if (stderr === true) {
		stderrHandlers = [process.stderr.write];
	} else if (typeof stderr === "function") {
		stderrHandlers = [stderr];
	} else if (Array.isArray(stderr)) {
		stderrHandlers = stderr;
	}
	if (maxOutputSize > 0) {
		stdoutHandlers.push((data) => result._addToBuffer(true, data));
		stderrHandlers.push((data) => result._addToBuffer(false, data));
	}

	subprocess.stdout!.setEncoding("utf8");
	subprocess.stdout!.on("data", (data) => {
		stdoutHandlers.forEach((handler) => handler(data));
	});
	subprocess.stderr!.setEncoding("utf8");
	subprocess.stderr!.on("data", (data) => {
		stderrHandlers.forEach((handler) => handler(data));
	});

	subprocess.on("spawn", () => {
		result.event.trigger("status", Status.running);
	});

	// Do not reject not to end up with an unhandled rejection.
	result.promise = new Promise<void>((resolve, reject) => {
		const handleError = () => {
			if (ignoreFailure) {
				resolve();
			} else {
				reject(new Exception("While executing: {}\n{}", cmds.join(" "), result.getOutput()));
			}
		};

		subprocess.on("error", (err) => {
			result.event.trigger("status", Status.failed);
			const message = "Process failed to start: " + String(err);
			stderrHandlers.forEach((handler) => handler(message));
			handleError();
		});

		subprocess.on("close", (code) => {
			if (typeof code === "number") {
				result.returncode = code;
				if (code == 0) {
					result.event.trigger("status", Status.completed);
					resolve();
				} else {
					result.event.trigger("status", Status.failed);
					const message = "Process failed with error code: " + code;
					stderrHandlers.forEach((handler) => handler(message));
					handleError();
				}
			} else {
				result.event.trigger("status", Status.cancelled);
				stderrHandlers.forEach((handler) => handler("Cancelled"));
				handleError();
			}
		});
	});

	return result;
}