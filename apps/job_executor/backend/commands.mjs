import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Context from "#bzd/apps/job_executor/backend/context.mjs";
import Args from "#bzd/apps/job_executor/backend/args.mjs";

import Executor from "#bzd/apps/job_executor/backend/executor.mjs";
import ExecutorShell from "#bzd/apps/job_executor/backend/executor_shell.mjs";
import ExecutorDocker from "#bzd/apps/job_executor/backend/executor_docker.mjs";

const Exception = ExceptionFactory("commands");
const Log = LogFactory("commands");

function makeExecutor(contextJob, schema) {
	if ("command" in schema) {
		return new ExecutorShell(contextJob);
	}
	if ("docker" in schema) {
		return new ExecutorDocker(contextJob);
	}
	Exception.unreachable("Undefined executor for this job: {:j}", schema);
}

export default class Commands {
	constructor(root) {
		this.context = new Context(root);
		this.executors = {};
	}

	async initialize() {
		await this.context.initialize();

		// Prefill the context
		for (const uid of this.context.getAllJobs()) {
			const contextJob = this.getContext(uid);
			const executor = new Executor(contextJob);
			await this.make(uid, executor);
		}

		// Start the info gathered thread.
		setInterval(async () => {
			await Promise.all(
				Object.entries(this.executors).map(async ([uid, executor]) => {
					try {
						const contextJob = this.getContext(uid);
						const info = await executor.getInfo();
						await contextJob.updateInfo(info);
					} catch (e) {
						Log.warning("Concurrent access while gathering info: {}", e);
					}
				}),
			);
		}, 5000);
	}

	/// Allocate a jobId for a command.
	async allocate() {
		const uid = this.context.allocate();
		return await this.context.addJob(uid);
	}

	async makeFromSchema(uid, schema, data) {
		const makeArgs = (visitor) => {
			const updatedData = Object.fromEntries(
				Object.entries(data).map(([key, value]) => {
					const item = schema.inputs.find((item) => item.name == key);
					return [key, visitor(item.type || "unknown", value, schema)];
				}),
			);
			const args = new Args(schema.args, updatedData);
			return visitor("post", args.process(), schema);
		};
		const contextJob = this.getContext(uid);
		const executor = makeExecutor(contextJob, schema);
		const args = makeArgs(executor.visitorArgs);
		await this.make(uid, executor, args);
	}

	/// Create a new command.
	async make(uid, executor, args = null) {
		Exception.assert(!(uid in this.executors), "The uid '{}' is already in use.", uid);
		this.executors[uid] = executor;
		if (args) {
			const contextJob = this.getContext(uid);
			await contextJob.updateInfo({
				args: args,
			});
		}
	}

	getExecutor(uid) {
		Exception.assertPrecondition(uid in this.executors, "Undefined job id '{}'.", uid);
		return this.executors[uid];
	}

	/// Run a specific command.
	async detach(uid) {
		const executor = this.getExecutor(uid);
		const contextJob = this.getContext(uid);
		const info = await contextJob.getInfo();
		Exception.assert("args" in info, "Information from uid '{}' is missing args: {:j}", uid, info);
		await executor.execute(uid, info.args);
		this.getContext(uid).captureOutput(executor);
	}

	/// Kill a specific command.
	async kill(uid) {
		const executor = this.getExecutor(uid);
		if (executor.kill) {
			await executor.kill();
		}
	}

	/// Remove a specific job.
	async remove(uid) {
		await this.kill(uid);
		await this.context.removeJob(uid);
		delete this.executors[uid];
	}

	getContext(uid) {
		return this.context.getJob(uid);
	}

	async getInfo(uid) {
		const contextJob = this.getContext(uid);
		return await contextJob.getInfo();
	}

	async getAllInfo() {
		const keys = this.context.getAllJobs();
		const infos = await Promise.all(keys.map((uid) => this.getInfo(uid)));
		return Object.fromEntries(infos.map((info, index) => [keys[index], info]));
	}

	installCommandWebsocket(context, uid) {
		const executor = this.getExecutor(uid);
		executor.installWebsocket(context);
	}
}
