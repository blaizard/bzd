import Commander from "commander";
import Path from "path";

import API from "bzd/core/api/server.mjs";
import APIv1 from "../api.v1.json";
import Web from "bzd/core/web.mjs";
import KeyValueStoreDisk from "bzd/db/key_value_store/disk.mjs";
import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import StorageDisk from "bzd/db/storage/disk.mjs";
import StorageDockerV2 from "bzd/db/storage/docker_v2.mjs";
import Cache from "bzd/core/cache.mjs";
import { CollectionPaging } from "bzd/db/utils.mjs";

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
		type: "disk",
		path: "/",
	});
	await keyValueStore.set("volumes", "docker", {
		type: "docker-v2",
		host: "https://docker.blaizard.com",
	});

	// Set the cache

	let cache = new Cache();

	/**
	 * Retrieve the storage implementation associated with this volume
	 */
	cache.register("volume", async (volume) => {
		const volumeInfo = await keyValueStore.get("volumes", volume, null);
		Exception.assert(volumeInfo !== null, "No volume are associated with this id: '{}'", volume);
		switch (volumeInfo.type) {
		case "disk":
			return new StorageDisk(volumeInfo.path);
		case "docker-v2":
			return new StorageDockerV2(volumeInfo.host);
		default:
			Exception.unreachable("Volume type unsupported: '{}'", volumeInfo.type);
		}
	});

	// Install the APIs

	let api = new API(APIv1, {
		channel: web,
	});

	function getInternalPath(path) {
		Exception.assert(typeof path == "string", "Path must be a string: '{}'", path);
		Exception.assert(path.search(/\.\./gi) === -1, "Path cannot contain '..': '{}'", path);
		const splitPath = path.split("/").filter((entry) => entry.length > 0);
		return { volume: splitPath[0], path: "/" + splitPath.slice(1).join("/") };
	}

	api.handle("get", "/list", async (inputs) => {
		// eslint-disable-next-line
		const { volume, path } = getInternalPath(inputs.path);
		const maxOrPaging = "paging" in inputs ? JSON.parse(inputs.paging) : 50;

		if (!volume) {
			const volumes = await keyValueStore.list("volumes", maxOrPaging);
			const data = Object.keys(volumes.data());
			const result = await CollectionPaging.makeFromList(data, data.length, (item) => {
				return { name: item, type: "bucket" };
			});
			return {
				data: result.data(),
				next: volumes.getNextPaging(),
			};
		}

		const storage = await cache.get("volume", volume);
		const result = await storage.list(path, maxOrPaging, /*includeMetadata*/ true);

		return {
			data: result.data(),
			next: result.getNextPaging(),
		};
	});

	Log.info("Application started");
	web.start();
})();
