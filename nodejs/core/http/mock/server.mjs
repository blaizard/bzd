import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Router from "#bzd/nodejs/core/router.mjs";
import MockServerContext from "#bzd/nodejs/core/http/mock/server_context.mjs";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.mjs";

const Log = LogFactory("http", "server", "mock");
const Exception = ExceptionFactory("http", "server", "mock");

export default class MockHttpServer {
	constructor() {
		this.started = false;
		this.routers = {};
		this.statistics = new StatisticsProvider("http.server");
	}

	async start() {
		this.started = true;
		Log.info("Mock web server started.");
	}

	stop() {
		this.started = false;
		Log.info("Mock web server stopped.");
	}

	async addStaticRoute(uri, path, options) {
		Exception.assert(this.started === false, "Cannot add new static routes if the server is started.");
	}

	addRoute(method, path, callback, options) {
		Exception.assert(this.started === false, "Cannot add new routes if the server is started.");

		const methodLower = method.toLowerCase();
		if (!(methodLower in this.routers)) {
			this.routers[methodLower] = new Router();
		}

		this.routers[methodLower].add(path, async (params, context) => {
			await callback(context.withParams(params));
		});
	}

	/// Send a request to the server.
	async send(method, path, request = {}) {
		// If the server is not started, requests are ignored.
		if (!this.started) {
			return;
		}

		const methodLower = method.toLowerCase();
		Exception.assert(method in this.routers, "Method {} is not available.", method);

		const context = MockServerContext.make(request).withPath(path);
		const result = await this.routers[methodLower].dispatch(context.request.path, context);
		Exception.assert(result !== false, "There is no handler for the endpoint {}.", path);

		// Handle redirects.
		if (context.response.redirect) {
			return await this.send(method, context.response.redirect, request);
		}

		return context.response;
	}

	/// Run a collection of tests.
	///
	/// \param tests A sequence of tests containing a request and expects key.
	async test(tests) {
		for (const test of tests) {
			const { request, expects } = test;
			Exception.assert(
				request.length == 2 || request.length == 3,
				"Request can only contain 2 or 3 entries: {:j}",
				request,
			);
			let message = "Testing " + request[0] + "::" + request[1];
			if (request[2]) {
				message += " (" + JSON.stringify(request[2]) + ")";
			}
			Log.info("{}, checking: {}", message, Object.keys(expects));
			const result = await this.send(...request);
			for (const [key, value] of Object.entries(expects)) {
				switch (key) {
					case "data":
						Exception.assertEqual(result.data, value);
						break;
					case "status":
						Exception.assertEqual(result.status, value);
						break;
					case "headers":
						for (const [headerName, headerValue] of Object.entries(value)) {
							Exception.assert(
								headerName in result.headers,
								"Expected header '{}' to be present in the response, response: {}",
								headerName,
								Object.keys(result.headers),
							);
							const valueOrValues = result.headers[headerName];
							const values = Array.isArray(valueOrValues) ? valueOrValues : [valueOrValues];
							const expectedValues = Array.isArray(headerValue) ? headerValue : [headerValue];
							Exception.assertEqual(
								values.sort(),
								expectedValues.sort(),
								"Expected header values for '{}'",
								headerName,
							);
						}
						break;
					default:
						Exception.unreachable("Unsupported expected type: {}", key);
				}
			}
		}
	}
}
