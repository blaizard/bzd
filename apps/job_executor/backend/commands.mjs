import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Executor from "#bzd/apps/job_executor/backend/executor.mjs";

const Exception = ExceptionFactory("commands");
const Log = LogFactory("commands");

export default class Commands {
	constructor(context) {
		this.context = context;
		this.contexts = {};

		// Prefill the context
		for (const [uid, contextJob] of Object.entries(this.context.getAllJobs())) {
			this.make(uid, contextJob);
		}
	}

	/// Allocate a jobId for a command.
	allocate() {
		return this.context.allocate();
	}

	/// Create a new command.
	make(uid, contextJob, makeArgs = (_type, arg) => arg) {
		Exception.assert(!(uid in this.contexts), "The uid '{}' is already in use.", uid);
		const context = {
			makeArgs: makeArgs,
			args: [],
			executor: new Executor(contextJob),
		};
		this.contexts[uid] = context;
	}

	get_(uid) {
		Exception.assert(uid in this.contexts, "Undefined job id '{}'.", uid);
		return this.contexts[uid];
	}

	/// Run a specific command.
	async detach(executor, uid, schema) {
		const context = this.get_(uid);
		context.executor = executor;
		context.args = context.makeArgs(context.executor.visitorArgs);
		await context.executor.execute(uid, schema, context.args);
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
		delete this.contexts[uid];
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
		const keys = Object.keys(this.contexts);
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
