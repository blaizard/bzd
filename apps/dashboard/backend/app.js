"use strict";

import Commander from "commander";

import API from "../../../nodejs/core/api.js";
import Web from "../../../nodejs/core/web.js";
import KeyValueStoreDisk from "../../../nodejs/core/key_value_store/disk.js";

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

	let api = new API();

	let keyValueStore = new KeyValueStoreDisk("/tmp/test/db");
	await keyValueStore.waitReady();
	await keyValueStore.set("bucket1", "key1", "Hello!");
	await keyValueStore.set("bucket1", "key2", "World!");
	await keyValueStore.delete("bucket2", "sds");

	web.addRoute("post", "/api/v1/sample", async (request, response) => {
		console.log("Received query: " + JSON.stringify(request.query));
		console.log("Received body: " + JSON.stringify(request.body));

		response.json({
			hello: "World"
		});
	}, Web.JSON);

	web.start();

})();
