import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("application");
const Log = LogFactory("application");

/// Wrapper class around an application.
export default class Application {
	constructor(uid, value) {
		this.uid = uid;
		this.value = value;
		this.modified = [];
	}
}
