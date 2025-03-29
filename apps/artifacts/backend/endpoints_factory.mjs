export default class EndpointsFactory {
	constructor() {
		this.data = {};
	}

	register(method, endpoints, handler, options = {}) {
		this.data[method] ??= [];
		this.data[method].push({
			paths: typeof endpoints === "string" ? [endpoints] : endpoints,
			handler: handler,
			options: options,
		});
	}

	unwrap() {
		return this.data;
	}
}
