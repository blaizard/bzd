import Commander from "commander";
import Path from "path";

import Filesystem from "bzd/core/filesystem.mjs";

import API from "bzd/core/api/server.mjs";
import APIv1 from "../api.v1.json";
import Web from "bzd/core/web.mjs";
import KeyValueStoreDisk from "bzd/db/key_value_store/disk.mjs";
import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import Cache from "bzd/core/cache.mjs";
import { CollectionPaging } from "bzd/db/utils.mjs";

import Plugins from "../plugins/backend.mjs";

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
		type: "fs",
		"fs.root": "/"
	});
	await keyValueStore.set("volumes", "docker.blaizard.com", {
		type: "docker-v2",
		host: "https://docker.blaizard.com"
	});
	await keyValueStore.set("volumes", "docker.gcr", {
		type: "docker-gcr",
		key: await Filesystem.readFile("/home/blaise/Downloads/blaizard-1295d2680329.json"),
		service: "gcr.io"
	});

	// Load all plugns
	Log.info("Using plugins: {}", Object.keys(Plugins).join(", "));

	// Set the cache
	let cache = new Cache();

	/**
	 * Retrieve the storage implementation associated with this volume
	 */
	cache.register("volume", async (volume) => {
		const params = await keyValueStore.get("volumes", volume, null);
		Exception.assert(params !== null, "No volume are associated with this id: '{}'", volume);
		Exception.assert("type" in params, "Unknown type for the volume: '{}'", volume);
		Exception.assert(params.type in Plugins, "Volume type unsupported: '{}'", params.type);
		Exception.assert("storage" in Plugins[params.type], "Storage not supported by plugin '{}'", params.type);
		const storage = Plugins[params.type].storage(params);
		await storage.waitReady();
		return storage;
	});

	// Start the proxy
	/*
	 *const proxy = new DockerV2Proxy("127.0.0.1:5050", 5050, await cache.get("volume", "docker.gcr"));
	 *await proxy.start();
	 */
	// Install the APIs

	let api = new API(APIv1, {
		channel: web
	});

	function getInternalPath(pathList) {
		Exception.assert(Array.isArray(pathList), "Path must be an array: '{:j}'", pathList);
		Exception.assert(
			pathList.every((path) => Boolean(path)),
			"Path elements cannot be empty: '{:j}'",
			pathList
		);
		return { volume: pathList[0], pathList: pathList.slice(1) };
	}

	api.handle("post", "/list", async (inputs) => {
		// eslint-disable-next-line
		const { volume, pathList } = getInternalPath(inputs.path);
		const maxOrPaging = "paging" in inputs ? inputs.paging : 50;

		if (!volume) {
			const volumes = await keyValueStore.list("volumes", maxOrPaging);
			const data = Object.keys(volumes.data());
			const result = await CollectionPaging.makeFromList(data, data.length, (item) => {
				return { name: item, type: "bucket" };
			});
			return {
				data: result.data(),
				next: volumes.getNextPaging()
			};
		}

		const storage = await cache.get("volume", volume);
		const result = await storage.list(pathList, maxOrPaging, /*includeMetadata*/ true);

		return {
			data: result.data(),
			next: result.getNextPaging()
		};
	});

	Log.info("Application started");
	web.start();
})();
