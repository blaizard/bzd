import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/local/command.mjs";

const Exception = ExceptionFactory("backend", "executor");
const Log = LogFactory("backend", "executor");

export default class Executor {
	constructor(contextJob) {
		this.contextJob = contextJob;
	}

	async getInfo() {
		return {};
	}

	visitorArgs(_type, arg) {
		return arg;
	}

	installWebsocket(context) {}
}
