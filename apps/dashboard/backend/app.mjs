"use strict";

import Commander from "commander";
import Path from "path";

import API from "bzd/core/api.mjs";
import APIv1 from "../api.v1.json";
import Cache from "bzd/core/cache.mjs";
import Web from "bzd/core/web.mjs";
import KeyValueStoreDisk from "bzd/core/key_value_store/disk.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import LogFactory from "bzd/core/log.mjs";
import { Source } from "../plugins/plugins.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

Commander.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("-p, --port <number>", "Port to be used to serve the application, can also be set with the environemnt variable BZD_PORT.", 8080, parseInt)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option("-d, --data <path>", "Where to store the data, can also be set with the environemnt variable BZD_PATH_DATA.", "/bzd/data")
	.parse(process.argv);

(async () => {

	// Read arguments
	const PORT = process.env.BZD_PORT || Commander.port;
	const PATH_STATIC = Commander.static;
	const PATH_DATA = process.env.BZD_PATH_DATA || Commander.data;

	// Set-up the web server
	let web = new Web(PORT);
	web.addStaticRoute("/", PATH_STATIC, "index.html");

	let keyValueStore = new KeyValueStoreDisk(Path.join(PATH_DATA, "db"));
	await keyValueStore.waitReady();

	let cache = new Cache();

	// Register plugins

	for (const type in Source) {
		Log.info("Register plugin '{}'", type);
		const plugin = (await Source[type].backend()).default;

		// Install plugin specific cache entries
		(plugin.cache || []).forEach((entry) => {
			Exception.assert(entry.collection, "Cache collection must be set.");
			Exception.assert(typeof entry.fetch === "function", "Cache fetch must be a function.");
			let options = {};
			if ("timeout" in entry) {
				options.timeout = entry.timeout;
			}
			cache.register(entry.collection, entry.fetch, options);
		});

		// Install plugin
		let options = {};
		if ("timeout" in Source[type]) {
			options.timeout = Source[type].timeout;
		}
		cache.register(type, async (uid) => {
		
			// Check that the UID exists and is of type jenkins
			const data = await keyValueStore.get("tiles", uid, null);
			Exception.assert(data !== null, "There is no data associated with UID '{}'.", uid);
			Exception.assert(data["source.type"] == type, "Data type mismatch, stored '{}' vs requested '{}'.", data.type, type);
	
			Log.debug("Plugin '{}' fetching for '{}'", type, uid);
			return await plugin.fetch(data, cache);

		}, options);
	}

	// Install the APIs

	let api = new API(APIv1);
	api.handle(web, "get", "/tiles", async () => {
		return await keyValueStore.list("tiles");
	});
	api.handle(web, "get", "/tile", async (inputs) => {
		return await keyValueStore.get("tiles", inputs.uid);
	});
	api.handle(web, "post", "/tile", async (inputs) => {
		await keyValueStore.set("tiles", null, inputs);
	});
	api.handle(web, "put", "/tile", async (inputs) => {
		await keyValueStore.set("tiles", inputs.uid, inputs.value);
	});
	api.handle(web, "get", "/data", async (inputs) => {
		return await cache.get(inputs.type, inputs.uid);
	});

	web.start();

})();
