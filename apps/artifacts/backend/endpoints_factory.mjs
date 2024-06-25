import HttpEndpoint from "#bzd/nodejs/core/http/endpoint.mjs";

export default class EndpointsFactory {
	constructor() {
		this.data = {};
	}

	register(method, endpoint, handler) {
		const regexpr = new HttpEndpoint(endpoint).toRegexp();
		this.data[method] ??= [];
		this.data[method].push({
			regexpr: regexpr,
			handler: handler,
		});
	}

	unwrap() {
		return this.data;
	}
}
