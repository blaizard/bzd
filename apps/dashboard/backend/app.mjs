import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import KeyValueStoreDisk from "#bzd/nodejs/db/key_value_store/disk.mjs";
import { Command } from "commander/esm.mjs";
import Path from "path";

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
		"Port to be used to serve the application, can also be set with the environemnt variable BZD_PORT.",
		8080,
		parseInt,
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option(
		"-d, --data <path>",
		"Where to store the data, can also be set with the environemnt variable BZD_PATH_DATA.",
		"/bzd/data",
	)
	.parse(process.argv);

async function getData(type, uid, cache) {
	const data = await cache.get("data", uid);

	// Check that the UID is of the right type
	Exception.assert(data["source.type"] == type, "Data type mismatch, stored '{}' vs requested '{}'.", data.type, type);
	return data;
}

(async () => {
	// Read arguments
	const PORT = Number(process.env.BZD_PORT || program.opts().port);
	const PATH_STATIC = program.opts().static;
	const PATH_DATA = process.env.BZD_PATH_DATA || program.opts().data;

	// Set-up the web server
	let web = new HttpServer(PORT);
	web.addStaticRoute("/", PATH_STATIC);

	let keyValueStore = await KeyValueStoreDisk.make(Path.join(PATH_DATA, "db"));

	let cache = new Cache();
	let events = {};

	// Register constructors
	cache.register(
		"data",
		async (uid) => {
			let data = await keyValueStore.get("tiles", uid, null);
			Exception.assert(data !== null, "There is no data associated with UID '{}'.", uid);
			return data;
		},
		{
			timeout: 60 * 60 * 1000, // 1h
		},
	);

	// Register plugins
	for (const type in Plugins) {
		if ("module" in Plugins[type]) {
			Log.info("Register plugin '{}'", type);
			const plugin = (await Plugins[type].module()).default;

			// Install plugin specific cache entries
			Object.entries(plugin.cache || {}).forEach((entry) => {
				const [collection, metadata] = entry;
				Exception.assert(collection, "Cache collection must be set.");
				Exception.assert(typeof metadata.fetch === "function", "Cache fetch must be a function.");
				let options = {};
				if ("timeout" in metadata) {
					options.timeout = metadata.timeout;
				}
				cache.register(collection, metadata.fetch, options);
			});

			// Install plugin
			let options = {};
			if ("timeout" in Plugins[type].metadata) {
				options.timeout = Plugins[type].metadata.timeout;
			}
			cache.register(
				type,
				async (uid) => {
					const data = await getData(type, uid, cache);
					Log.debug("Plugin '{}' fetching for '{}'", type, uid);
					return await plugin.fetch(data, cache);
				},
				options,
			);

			// Install events
			if ("events" in plugin) {
				Log.info("Register events for '{}'", type);
				events[type] = plugin.events;
			}
		}
	}

	// Install the APIs

	let api = new RestServer(APIv1.rest, {
		channel: web,
	});
	api.handle("get", "/tiles", async () => {
		const result = await keyValueStore.list("tiles");
		return result.data();
	});
	api.handle("get", "/tile", async (inputs) => {
		return await keyValueStore.get("tiles", inputs.uid);
	});
	api.handle("post", "/tile", async (inputs) => {
		await keyValueStore.set("tiles", null, inputs.value);
		await cache.setDirty("data", inputs.uid);
	});
	api.handle("put", "/tile", async (inputs) => {
		await keyValueStore.set("tiles", inputs.uid, inputs.value);
		await cache.setDirty("data", inputs.uid);
	});
	api.handle("delete", "/tile", async (inputs) => {
		await keyValueStore.delete("tiles", inputs.uid);
	});
	api.handle("get", "/data", async (inputs) => {
		return await cache.get(inputs.type, inputs.uid);
	});
	api.handle("post", "/event", async (inputs) => {
		Exception.assert(inputs.type in events, "No event associated with '{}'.", inputs.type);
		Exception.assert(
			inputs.event in events[inputs.type],
			"'{}' is not a valid event for '{}'.",
			inputs.event,
			inputs.type,
		);
		const data = await getData(inputs.type, inputs.uid, cache);
		await events[inputs.type][inputs.event](data, cache, inputs.args);

		// Invalidates the cache
		await cache.setDirty(inputs.type, inputs.uid);
		return await cache.get(inputs.type, inputs.uid);
	});

	web.start();
})();
