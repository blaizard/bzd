

import ExceptionFactory from "../../exception.mjs";
export default class ExceptionFetch extends ExceptionFactory("fetch", "impl") {
	constructor (code, ...args) {
		super(...args);
		this.code = code;
	}
}
