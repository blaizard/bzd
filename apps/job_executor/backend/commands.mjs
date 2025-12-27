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
		this.jobs = {};
	}

	async initialize() {
		await this.context.initialize();

		// Prefill the context
		for (const uid of this.context.getAllJobs()) {
			const contextJob = this.getContext(uid);
			const executor = new Executor(contextJob);
			this.make(uid, executor, []);
		}
	}

	/// Allocate a jobId for a command.
	async allocate() {
		const uid = this.context.allocate();
		return await this.context.addJob(uid);
	}

	makeFromSchema(uid, schema, data) {
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
		this.make(uid, executor, args);
	}

	/// Create a new command.
	make(uid, executor, args) {
		Exception.assert(!(uid in this.jobs), "The uid '{}' is already in use.", uid);
		const context = {
			args: args,
			executor: executor,
		};
		this.jobs[uid] = context;
	}

	get_(uid) {
		Exception.assert(uid in this.jobs, "Undefined job id '{}'.", uid);
		return this.jobs[uid];
	}

	/// Run a specific command.
	async detach(uid) {
		const context = this.get_(uid);
		await context.executor.execute(uid, context.args);
	}

	/// Kill a specific command.
	async kill(uid) {
		const context = this.get_(uid);
		if (context.executor.kill) {
			await context.executor.kill();
		}
	}

	/// Remove a specific job.
	async remove(uid) {
		await this.kill(uid);
		await this.context.removeJob(uid);
		delete this.jobs[uid];
	}

	getContext(uid) {
		return this.context.getJob(uid);
	}

	async getInfo(uid) {
		const context = this.get_(uid);
		let args = {
			args: context.args,
		};
		if (context.executor) {
			Object.assign(args, await context.executor.getInfo());
		}
		return args;
	}

	async getAllInfo() {
		const keys = Object.keys(this.jobs);
		const infos = await Promise.all(keys.map((uid) => this.getInfo(uid)));
		return Object.fromEntries(infos.map((info, index) => [keys[index], info]));
	}

	installCommandWebsocket(context, uid) {
		const job = this.get_(uid);
		if (job.executor) {
			job.executor.installWebsocket(context);
		}
	}
}
