"use strict";

const Web = require("../../../nodejs/require/web.js");
const Exception = require("../../../nodejs/require/exception.js")("backend");
const KeyValueStoreDisk = require("../../../nodejs/require/key_value_store/disk.js");
const Commander = require("commander");

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
	await keyValueStore.set("bucket1", "key1", "Hello!");
	await keyValueStore.set("bucket1", "key2", "World!");
	await keyValueStore.delete("bucket2", "sds");

	web.start();
})();