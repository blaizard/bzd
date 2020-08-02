import Commander from "commander";
import Path from "path";

import API from "bzd/core/api/server.mjs";
import APIv1 from "../api.v1.json";
import Web from "bzd/core/web.mjs";
import KeyValueStoreDisk from "bzd/db/key_value_store/disk.mjs";
import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";

const Log = LogFactory("backend");
const Exception = ExceptionFactory("backend");

Commander.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with the environemnt variable BZD_PORT.",
		8080,
		parseInt
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option(
		"-d, --data <path>",
		"Where to store the data, can also be set with the environemnt variable BZD_PATH_DATA.",
		"/bzd/data"
	)
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

	// Test data
	await keyValueStore.set("volumes", "disk", {
		"type": "disk",
		"path": "/"
	});

	// Install the APIs

	let api = new API(APIv1, {
		channel: web,
	});

	function getInternalPath(path) {
		Exception.assert(typeof path == "string", "Path must be a string: '{}'", path);
		Exception.assert(path.search(/[^a-z0-9\/_.]/ig) === -1, "Malformed input path: '{}'", path);
		Exception.assert(path.search(/\.\./ig) === -1, "Path cannot contain '..': '{}'", path);
		const splitPath =  path.split("/").filter((entry) => entry.length > 0);
		return {volume: splitPath[0], path: "/" + splitPath.slice(1).join("/")};
	}

	api.handle("get", "/list", async (inputs) => {
		const path = getInternalPath(inputs.path);
		console.log(path);
		return {};
	});

	Log.info("Application started");
	web.start();
})();
