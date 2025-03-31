import ExceptionFactory from "../exception.mjs";
import Router from "#bzd/nodejs/core/router.mjs";

const Exception = ExceptionFactory("http", "server", "endpoint");

/// Parse an endpoint.
///
/// Valid schema are as follow:
/// /hello/{world}/my/{path:*}
export default class HttpEndpoint {
	constructor(endpoint) {
		this.compiledRoute = Router.toCompiledRoute(endpoint);
	}

	match(path) {
		return this.compiledRoute.match(path);
	}

	toRegexp() {
		return this.compiledRoute.regexpr;
	}

	toRoute(values) {
		return this.compiledRoute.toRoute(values);
	}
}
