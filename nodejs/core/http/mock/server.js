import { ExceptionFactory, ExceptionPrecondition } from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Router from "#bzd/nodejs/core/router.js";
import MockServerContext from "#bzd/nodejs/core/http/mock/server_context.js";
import StatisticsProvider from "#bzd/nodejs/core/statistics/provider.js";
import { HttpError } from "#bzd/nodejs/core/http/server_context.js";

const Log = LogFactory("http", "server", "mock");
const Exception = ExceptionFactory("http", "server", "mock");

export default class MockHttpServer {
	constructor(config) {
		this.started = false;
		this.routers = {};
		this.statistics = new StatisticsProvider("http.server");
		this.config = Object.assign(
			{
				/// Authentication object.
				authentication: null,
			},
			config,
		);
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
		// Update the options
		options = Object.assign(
			{
				/// Authentication constraints/scopes.
				authentication: null,
			},
			options,
		);

		Exception.assert(this.started === false, "Cannot add new routes if the server is started.");
		if (options.authentication !== null) {
			Exception.assert(
				this.config.authentication,
				"The route {}::{} has authentication requirement but no authentication object was specified.",
				method,
				path,
			);
		}

		const methodLower = method.toLowerCase();
		if (!(methodLower in this.routers)) {
			this.routers[methodLower] = new Router();
		}

		this.routers[methodLower].add(path, async (params, context) => {
			try {
				// Check if this request needs authentication.
				if (options.authentication) {
					context.session = await this.config.authentication.verify(context, options.authentication);
					if (!context.session) {
						throw this.config.authentication.httpErrorUnauthorized(/*requestAuthentication*/ true);
					}
				}

				await callback(context.withParams(params));
			} catch (e) {
				if (e instanceof HttpError) {
					e.send(context);
				} else if (e instanceof ExceptionPrecondition) {
					context.sendStatus(400, e.message);
				} else {
					throw e;
				}
			}
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
				"Request can only contain 2 or 3 entries: {:?}",
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
					case "data_contains":
						Exception.assertEqual(typeof result.data, "string", "Expected data to be a string.");
						Exception.assert(
							result.data.includes(value),
							"Expected data to contain '{}', data: {}",
							value,
							result.data,
						);
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
