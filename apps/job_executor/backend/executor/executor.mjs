import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Event from "#bzd/nodejs/core/event.mjs";

import ExecutorReadonly from "#bzd/apps/job_executor/backend/executor/executor_readonly.mjs";
import ExecutorShell from "#bzd/apps/job_executor/backend/executor/executor_shell.mjs";
import ExecutorDocker from "#bzd/apps/job_executor/backend/executor/executor_docker.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");

/// Top level class for executors, this is a wrapper around all available executors.
///
/// It connects the executor with their context.
export default class Executor {
	static ExecutorClasses = {
		readonly: ExecutorReadonly,
		shell: ExecutorShell,
		docker: ExecutorDocker,
	};

	constructor(executor, options) {
		this.executor = executor;
		this.options = Object.assign(
			{
				/// Maximum number of bytes in the stdout.
				maxOutputSize: 10000,
			},
			options,
		);

		this.output = [];
		this.outputSize = 0;
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

	static async make(uid, type, contextJob) {
		Exception.assert(type in Executor.ExecutorClasses, "No executor type '{}' available.", type);
		const executor = new Executor.ExecutorClasses[type](uid, contextJob);
		return await Executor.makeFromExecutor(executor, contextJob);
	}

	static async makeFromExecutor(executor, maybeContextJob) {
		const wrapper = new Executor(executor);

		if (maybeContextJob) {
			await maybeContextJob.getLogs((line) => {
				wrapper.event.trigger("output", line);
			});
			maybeContextJob.captureOutput(executor);
		}

		if (wrapper.executor.installWebsocket) {
			// Capture the executor output.
			class Capture {
				constructor(event) {
					this.event = event;
				}
				send(data) {
					this.event.trigger("output", data);
				}
				read(onRead) {
					this.event.on("input", onRead);
				}
				exit() {}
			}
			wrapper.executor.installWebsocket(new Capture(wrapper.event));
		}

		return wrapper;
	}

	/// Discover existing jobs if any.
	static async discover(context) {
		let discovered = {};
		for (const ExecutorClass of Object.values(Executor.ExecutorClasses)) {
			const executors = await ExecutorClass.discover(context);
			for (const [uid, executor] of Object.entries(executors)) {
				Exception.assert(!(uid in discovered), "The executor '{}' was discovered twice.", uid);
				discovered[uid] = await Executor.makeFromExecutor(executor, context.getJob(uid, null));
			}
		}
		return discovered;
	}

	async execute(args) {
		await this.executor.execute(args);
	}

	async kill() {
		if (this.executor.kill) {
			return await this.executor.kill();
		}
	}

	async getInfo() {
		return await this.executor.getInfo();
	}

	visitorArgs(type, arg, schema) {
		return this.executor.visitorArgs(type, arg, schema);
	}

	installWebsocket(context) {
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
		context.read((data) => this.event.trigger("input", data));
	}
}
