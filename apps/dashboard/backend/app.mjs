import Cache from "#bzd/nodejs/core/cache.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import config from "#bzd/apps/dashboard/backend/config.json" with { type: "json" };
import { makeUid } from "#bzd/nodejs/utils/uid.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Plugins from "#bzd/apps/dashboard/plugins/plugins.backend.index.mjs";

import Backend from "#bzd/nodejs/vue/apps/backend.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

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
	// Backend
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useServices()
		.useStatistics()
		.useLoggerMemory()
		.setup();

	let cache = new Cache({
		garbageCollector: !backend.test,
	});
	let plugins = {};
	let pluginClasses = {};
	let events = {};
	let layout = [];

	// Get the plugin type of an UID.
	const uidToType = (uid, mustExist) => {
		Exception.assertPrecondition(uid in plugins, "The plugin '{}' does not exists.", uid);
		if (!("source.type" in plugins[uid].config)) {
			Exception.assertPrecondition(
				!mustExist,
				"The plugin '{}' configuration {:j} is missing a 'source.type'.",
				uid,
				plugins[uid].config,
			);
			return null;
		}
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
	for (const data of config.layout) {
		switch (data.type || "tile") {
			case "tile":
				const uid = makeUid();

				// Set the structure.
				plugins[uid] = {
					instance: null,
					config: data,
				};

				// Identify the plugin type.
				const pluginType = uidToType(uid, /*mustExist*/ false);
				// If the plugin has a backend.
				if (pluginType) {
					Exception.assert(pluginType in pluginClasses, "The plugin of type {} does not exists.", pluginType);
					Log.info("Creating '{}' from plugin '{}'.", uid, pluginType);

					// Event factory.
					const eventsFactory = new EventsFactory(events, uid);

					// Instantiate the plugin.
					plugins[uid].instance = new pluginClasses[pluginType](data, eventsFactory);
				} else {
					Log.info("Creating '{}'.", uid);
				}

				layout.push(
					Object.assign(
						{
							type: "tile",
							uid: uid,
						},
						data,
					),
				);
				break;
			case "separator":
				layout.push(data);
				break;
			default:
				Exception.error("Unsupported layout element type '{}'.", data.type);
		}
	}

	cache.register(
		"check-url",
		async (url) => {
			try {
				await HttpClient.request(url, {
					method: "get",
				});
				return true;
			} catch (e) {
				return false;
			}
		},
		{
			timeout: 1000,
		},
	);

	// Install the REST APIs.

	backend.rest.handle("get", "/check-url", async (inputs) => {
		return { valid: await cache.get("check-url", inputs.url) };
	});
	backend.rest.handle("get", "/layout", async () => {
		return {
			layout: layout,
		};
	});
	backend.rest.handle("get", "/instance", async (inputs) => {
		return plugins[inputs.uid].config;
	});
	backend.rest.handle("get", "/data", async (inputs) => {
		const pluginType = uidToType(inputs.uid, /*mustExist*/ true);
		return await cache.get(pluginType, inputs.uid);
	});
	backend.rest.handle("post", "/event", async (inputs) => {
		Exception.assert(inputs.uid in events, "No event associated with '{}'.", inputs.uid);
		Exception.assert(
			inputs.event in events[inputs.uid],
			"'{}' is not a valid event for '{}'.",
			inputs.event,
			inputs.uid,
		);
		const pluginType = uidToType(inputs.uid, /*mustExist*/ true);

		await events[inputs.uid][inputs.event](cache, inputs.args);

		// Invalidates the cache
		await cache.setDirty(pluginType, inputs.uid);
		return await cache.get(pluginType, inputs.uid);
	});

	await backend.start();

	if (backend.test) {
		await backend.web.test(config.tests || []);
		await backend.stop();
	}
})();
