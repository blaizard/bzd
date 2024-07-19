import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { Command } from "commander/esm.mjs";
import config from "#bzd/apps/dashboard/backend/config.json" assert { type: "json" };
import { makeUid } from "#bzd/nodejs/utils/uid.mjs";
import APIv1 from "#bzd/apps/dashboard/api.v1.json" assert { type: "json" };
import Plugins from "#bzd/apps/dashboard/plugins/plugins.backend.index.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with the environment variable BZD_PORT.",
		8080,
		parseInt,
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option(
		"-d, --data <path>",
		"Where to store the data, can also be set with the environment variable BZD_PATH_DATA.",
		"/bzd/data",
	)
	.parse(process.argv);

class EventsFactory {
	constructor(events, uid) {
		Exception.assert(!(uid in events), "Events for '{}' are already registered.", uid);
		events[uid] = {};
		this.uid = uid;
		this.events = events[uid];
	}

	register(name, callback) {
		Exception.assert(!(name in this.events), "The event '{}' for '{}' is already registered.", name, this.uid);
		this.events[name] = callback;
	}
}

(async () => {
	// Read arguments
	const PORT = Number(process.env.BZD_PORT || program.opts().port);
	const PATH_STATIC = program.opts().static;
	const PATH_DATA = process.env.BZD_PATH_DATA || program.opts().data;

	// Set-up the web server
	let web = new HttpServer(PORT);
	web.addStaticRoute("/", PATH_STATIC);

	let cache = new Cache();
	let plugins = {};
	let pluginClasses = {};
	let events = {};

	// Get the plugin type of an UID.
	const uidToType = (uid) => {
		Exception.assertPrecondition(uid in plugins, "The plugin '{}' does not exists.", uid);
		Exception.assert(
			"source.type" in plugins[uid].config,
			"The plugin '{}' configuration {:j} is missing a 'source.type'.",
			uid,
			plugins[uid].config,
		);
		return plugins[uid].config["source.type"];
	};

	// Register plugins.
	for (const [type, data] of Object.entries(Plugins)) {
		// Only handle backend modules.
		if ("module" in data) {
			const PluginClass = (await data.module()).default;
			pluginClasses[type] = PluginClass;

			// Register cache entries.
			PluginClass.register(cache);

			// Register cache for the plugins.
			let options = {};
			if ("timeout" in data.metadata) {
				options.timeout = data.metadata.timeout;
			}
			cache.register(
				type,
				async (uid) => {
					Log.debug("Plugin '{}' fetching for '{}'", type, uid);
					Exception.assert(uid in plugins, "Plugin '{}' does not exists.", uid);
					return await plugins[uid].instance.fetch(cache);
				},
				options,
			);
		}
	}

	// Register plugin instances.
	for (const data of config.instances) {
		const uid = makeUid();

		// Set the structure.
		plugins[uid] = {
			instance: null,
			config: data,
		};

		// Identify the plugin type.
		const pluginType = uidToType(uid);
		Exception.assert(pluginType in pluginClasses, "The plugin of type {} does not exists.", pluginType);
		Log.info("Creating '{}' from plugin '{}'.", uid, pluginType);

		// Event factory.
		const eventsFactory = new EventsFactory(events, uid);

		// Instantiate the plugin.
		plugins[uid].instance = new pluginClasses[pluginType](data, eventsFactory);
	}

	// Install the APIs

	let api = new RestServer(APIv1.rest, {
		channel: web,
	});
	api.handle("get", "/instances", async () => {
		return Object.entries(plugins).reduce((object, [uid, data]) => {
			object[uid] = data.config;
			return object;
		}, {});
	});
	api.handle("get", "/instance", async (inputs) => {
		return plugins[inputs.uid].config;
	});
	api.handle("get", "/data", async (inputs) => {
		const pluginType = uidToType(inputs.uid);
		return await cache.get(pluginType, inputs.uid);
	});
	api.handle("post", "/event", async (inputs) => {
		Exception.assert(inputs.uid in events, "No event associated with '{}'.", inputs.uid);
		Exception.assert(
			inputs.event in events[inputs.uid],
			"'{}' is not a valid event for '{}'.",
			inputs.event,
			inputs.uid,
		);
		const pluginType = uidToType(inputs.uid);

		await events[inputs.uid][inputs.event](cache, inputs.args);

		// Invalidates the cache
		await cache.setDirty(pluginType, inputs.uid);
		return await cache.get(pluginType, inputs.uid);
	});

	web.start();
})();
