"use strict";

import Commander from "commander";

import API from "../../../nodejs/core/api.js";
import Cache from "../../../nodejs/core/cache.js";
import FileSystem from "../../../nodejs/core/filesystem.js";
import Web from "../../../nodejs/core/web.js";
import KeyValueStoreDisk from "../../../nodejs/core/key_value_store/disk.js";
import ExceptionFactory from "../../../nodejs/core/exception.js";
import LogFactory from "../../../nodejs/core/log.js";
import { Source } from "../plugins/plugins.js";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

Commander.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("-p, --port <number>", "Port to be used to serve the application.", 8080, parseInt)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.parse(process.argv);

(async () => {

	// Set-up the web server
	let web = new Web(Commander.port, {
		rootDir: Commander.static
	});

	let keyValueStore = new KeyValueStoreDisk("/tmp/test/db");
	await keyValueStore.waitReady();

	let cache = new Cache();

	// Register plugins

	for (const type in Source) {
		Log.info("Register plugin '{}'", type);
		const plugin = (await Source[type].backend()).default;
		let options = {};
		if ("timeout" in Source[type]) {
			options.timeout = Source[type].timeout;
		}
		cache.register(type, async (uid) => {
		
			// Check that the UID exists and is of type jenkins
			const data = await keyValueStore.get("tiles", uid, null);
			Exception.assert(data !== null, "There is no data associated with UID '{}'.", uid);
			Exception.assert(data["source.type"] == type, "Data type mismatch, stored '{}' vs requested '{}'.", data.type, type);
	
			Log.info("Plugin '{}' fetching for '{}'", type, uid);
			return await plugin.fetch(data);

		}, options);
	}

	// Install the APIs

	const apiV1Description = JSON.parse(await FileSystem.readFile("./apps/dashboard/api.v1.json"));
	let api = new API(apiV1Description);
	api.handle(web, "get", "/tiles", async () => {
		return await keyValueStore.list("tiles");
	});
	api.handle(web, "post", "/tile", async (inputs) => {
		await keyValueStore.set("tiles", null, inputs);
	});
	api.handle(web, "get", "/data", async (inputs) => {
		return await cache.get(inputs.type, inputs.uid);
	});

	web.start();

})();
