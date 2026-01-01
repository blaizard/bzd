import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Context from "#bzd/apps/job_executor/backend/context.mjs";
import Args from "#bzd/apps/job_executor/backend/args.mjs";
import ServicesProvider from "#bzd/nodejs/core/services/provider.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";

import Executor from "#bzd/apps/job_executor/backend/executor/executor.mjs";

const Exception = ExceptionFactory("commands");
const Log = LogFactory("commands");

export default class Commands {
	constructor(root, options) {
		this.context = new Context(root);
		this.options = Object.assign(
			{
				services: new ServicesProvider(),
			},
			options,
		);
		this.executors = {};
	}

	async initialize() {
		await this.context.initialize();

		const executors = await Executor.discover(this.context);
		for (const [uid, executor] of Object.entries(executors)) {
			await this.make(uid, executor);
		}

		// Prefill the entries that are have not been discovered.
		for (const uid of this.context.getAllJobs()) {
			if (!(uid in this.executors)) {
				const contextJob = this.getContext(uid);
				const info = await contextJob.getInfo();
				// Jobs that are in idle status and that have a type can be matched with an executor.
				if (info.status == Status.idle) {
					const executor = await Executor.make(uid, info.type, contextJob);
					await this.make(uid, executor);
				} else {
					const executor = await Executor.make(uid, "readonly", contextJob);
					await this.make(uid, executor);
				}
			}
		}
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
		const executor = await Executor.make(uid, schema.type, contextJob);
		const args = makeArgs((...args) => executor.visitorArgs(...args));
		await this.make(uid, executor, {
			args: args,
			status: Status.idle,
		});
	}

	/// Create a new command.
	async make(uid, executor, info = {}) {
		Exception.assert(!(uid in this.executors), "The uid '{}' is already in use.", uid);
		this.executors[uid] = executor;

		// Update the information.
		if (executor.constructor.type) {
			info.type = executor.constructor.type;
		}
		if (Object.keys(info).length > 0) {
			const maybeContextJob = this.getContext(uid);
			if (maybeContextJob) {
				await maybeContextJob.updateInfo(info);
			}
		}
	}

	getExecutor(uid) {
		Exception.assertPrecondition(uid in this.executors, "Undefined job id '{}'.", uid);
		return this.executors[uid];
	}

	/// Run a specific command.
	async detach(uid) {
		const executor = this.getExecutor(uid);
		const info = await executor.getInfo();
		Exception.assertPrecondition(info.status == Status.idle, "This job already started: {:j}", uid);
		Exception.assert("args" in info, "Information from uid '{}' is missing args: {:j}", uid, info);
		await executor.execute(info.args);
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

	getContext(uid, valueOr) {
		return this.context.getJob(uid, valueOr);
	}

	async getInfo(uid) {
		const executor = this.getExecutor(uid);
		return await executor.getInfo();
	}

	async getAllInfo() {
		const uids = Object.keys(this.executors);
		const infos = await Promise.all(uids.map((uid) => this.getInfo(uid)));
		return Object.fromEntries(infos.map((info, index) => [uids[index], info]));
	}

	installCommandWebsocket(context, uid) {
		const executor = this.getExecutor(uid);
		executor.installWebsocket(context);
	}
}
