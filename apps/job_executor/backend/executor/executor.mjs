import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Event from "#bzd/nodejs/core/event.mjs";
import Lock from "#bzd/nodejs/core/lock.mjs";

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

	constructor(executor, maybeContextJob, options) {
		this.executor = executor;
		this.maybeContextJob = maybeContextJob;
		this.options = Object.assign(
			{
				/// Maximum number of bytes in the stdout.
				maxOutputSize: 10000,
			},
			options,
		);
		this.lock = new Lock();

		this.info = {
			type: this.executor.constructor.type || null,
		};
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

	/// Factory to make an executor from its type.
	static async make(uid, type, contextJob) {
		Exception.assert(type in Executor.ExecutorClasses, "No executor type '{}' available.", type);
		const executor = new Executor.ExecutorClasses[type](uid, contextJob);
		return await Executor.makeFromExecutor(executor, contextJob);
	}

	/// Factory to make an executor from a typed-executor.
	static async makeFromExecutor(executor, maybeContextJob) {
		const wrapper = new Executor(executor, maybeContextJob);
		await wrapper.initialize();
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

	async initialize() {
		await this.lock.acquire(async () => {
			// If a context is associated with this executor.
			if (this.maybeContextJob) {
				await this.maybeContextJob.getLogs((line) => {
					this.event.trigger("output", line);
				});
				this.maybeContextJob.captureOutput(this.executor);
			}

			// If the executor supports websockets.
			if (this.executor.installWebsocket) {
				this.executor.installWebsocket({
					send: (data) => {
						this.event.trigger("output", data);
					},
					read: (onRead) => {
						this.event.on("input", onRead);
					},
					exit: () => {},
				});
			}
		});
	}

	writeToStdin(data) {
		this.event.trigger("input", JSON.stringify({ type: "stream", value: data }));
	}

	async execute(args) {
		await this.lock.acquire(async () => {
			await this.executor.execute(args);
		});
	}

	async kill() {
		await this.lock.acquire(async () => {
			if (this.executor.kill) {
				await this.executor.kill();
			}
		});
	}

	async getInfo() {
		return await this.lock.acquire(async () => {
			const info = await this.executor.getInfo();
			return await this.updateInfo(info, /*acquire*/ false);
		});
	}

	async updateInfo(info, acquire = true) {
		Object.assign(this.info, info);

		// Save and merge this information with the persistent one.
		const acquireFct = acquire ? (callback) => this.lock.acquire(callback) : (callback) => callback();
		await acquireFct(async () => {
			if (this.maybeContextJob) {
				this.info = await this.maybeContextJob.updateInfo(this.info);
			}
		});

		return this.info;
	}

	async remove() {
		await this.lock.acquire(async () => {
			if (this.maybeContextJob) {
				await this.maybeContextJob.destroy();
				this.maybeContextJob = null;
			}
		});
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
