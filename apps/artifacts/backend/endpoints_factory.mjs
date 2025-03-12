export default class EndpointsFactory {
	constructor() {
		this.data = {};
	}

	register(method, endpoint, handler, options = {}) {
		this.data[method] ??= [];
		this.data[method].push({
			path: endpoint,
			handler: handler,
			options: options,
		});
	}

	unwrap() {
		return this.data;
	}
}
