import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("commands");
const Log = LogFactory("commands");

export default class Commands {
	constructor() {
		this.contexts = {};
		this.uid = 0;
	}

	/// Allocate a jobId for a command.
	allocate() {
		return ++this.uid;
	}

	/// Create a new command.
	make(uid, makeArgs) {
		Exception.assert(!(uid in this.contexts), "The uid '{}' is already in use.", uid);
		const context = {
			makeArgs: makeArgs,
			args: [],
			executor: null,
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
		Exception.assert(context.executor === null, "This uid '{}' has already been executed.", uid);
		context.executor = executor;
		context.args = context.makeArgs(context.executor.visitorArgs);
		await context.executor.execute(uid, schema, context.args);
	}

	/// Kill a specific command.
	async kill(uid) {
		await this.get_(uid).executor.kill();
	}

	async getInfo(uid) {
		const context = this.get_(uid);
		return Object.assign(
			{
				args: context.args,
			},
			await context.executor.getInfo(),
		);
	}

	async getAllInfo() {
		const keys = Object.keys(this.contexts);
		const infos = await Promise.all(keys.map((uid) => this.getInfo(uid)));
		return Object.fromEntries(infos.map((info, index) => [keys[index], info]));
	}

	installCommandWebsocket(context, uid) {
		this.get_(uid).executor.installWebsocket(context);
	}
}
