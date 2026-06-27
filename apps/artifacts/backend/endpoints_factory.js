export default class EndpointsFactory {
	constructor() {
		this.data = {
			rest: {},
			mcp: {},
		};
	}

	register(method, endpoint, handler, options = {}) {
		this.data.rest[method] ??= [];
		this.data.rest[method].push({
			path: endpoint,
			handler: handler,
			options: options,
		});
	}

	registerMCP(endpoint, handler, schema, options = {}) {
		this.data.mcp[endpoint] = {
			handler: handler,
			schema: schema,
			options: options,
		};
	}

	unwrap() {
		return this.data;
	}
}
