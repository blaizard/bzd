import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";
import ServiceProvider from "#bzd/nodejs/core/services/provider.mjs";
import EndpointsFactory from "#bzd/apps/artifacts/backend/endpoints_factory.mjs";
import Router from "#bzd/nodejs/core/router.mjs";
import ServerContext from "#bzd/nodejs/core/http/mock/server_context.mjs";

const Exception = ExceptionFactory("plugin", "tester");

export default class PluginTester {
	constructor() {
		this.plugins = {};
		this.services = new Services();
	}

	/// Register a plugin for testing.
	///
	/// \param volume The name of the volume.
	/// \param PluginType The plugin class to be tested.
	/// \param options A dictionary of options to be passed to the plugin instance.
	/// \param components Additional components to be mocked.
	register(volume, PluginType, options = {}, components = undefined) {
		Exception.assert(!(volume in this.plugins), "Volume {} already registered.", volume);

		const provider = new ServiceProvider(volume);
		const endpoints = new EndpointsFactory();
		const instance = new PluginType(volume, options, provider, endpoints, components);
		let routers = {};
		for (const [method, dataList] of Object.entries(endpoints.unwrap())) {
			routers[method] ??= new Router();
			for (const data of dataList) {
				routers[method].add(data.path, async (params, context) => {
					context.withParams(params);
					await data.handler(context);
				});
			}
		}
		this.plugins[volume] = {
			instance: instance,
			endpoints: endpoints,
			routers: routers,
		};
		this.services.register(provider, volume);
	}

	async start() {
		await this.services.start();
	}

	async stop() {
		await this.services.stop();
	}

	async send(volume, method, path, request = {}, throwOnFailure = true) {
		Exception.assert(volume in this.plugins, "Volume {} doesn't exist.", volume);
		const routers = this.plugins[volume].routers;
		Exception.assert(method in routers, "Method {} is not available in the plugin {}.", method, volume);

		const context = ServerContext.make(request).withPath(path);
		const result = await routers[method].dispatch(context.request.path, context);
		Exception.assert(result !== false, "There is no handler for the endpoint {} for plugin {}.", path, volume);
		Exception.assert(
			!throwOnFailure || (context.response.status >= 200 && context.response.status <= 299),
			"Invalid result status: {}",
			context.response.status,
		);

		return context.response;
	}
}
