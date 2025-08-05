import ExceptionFactory from "../exception.mjs";
import Router from "#bzd/nodejs/core/router.mjs";

const Exception = ExceptionFactory("http", "endpoints");

/// Handle multiple endpoints
export default class HttpEndpoints {
	constructor(endpoints) {
		this.router = new Router();
		for (const endpoint of endpoints) {
			this.router.add(endpoint, () => {}, { endpoint: endpoint });
		}
	}

	match(path) {
		const match = this.router.match(path);
		Exception.assert(match !== false, "There is no match for path: '{}'.", path);
		return match.args.endpoint;
	}
}
