import { Command } from "commander/esm.mjs";
import Path from "path";

import API from "bzd/core/api/server.mjs";
import APIv1 from "../api.v1.json";
import HttpServer from "bzd/core/http/server.mjs";
import KeyValueStoreDisk from "bzd/db/key_value_store/disk.mjs";
import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import Authentication from "bzd/core/authentication/token/server.mjs";
import Cache from "bzd/core/cache.mjs";
import { CollectionPaging } from "bzd/db/utils.mjs";
import Services from "./services.mjs";
import Plugins from "../plugins/backend.mjs";
import Permissions from "bzd/db/storage/permissions.mjs";

const Log = LogFactory("backend");
const Exception = ExceptionFactory("backend");

// For debugging purposes.
// Log.setLevel("debug");

const program = new Command();
program
	.version("1.0.0", "-v, --version")
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
	.option("--test", "Use test data.")
	.parse(process.argv);

(async () => {
	// Read arguments
	const PORT = process.env.BZD_PORT || program.opts().port;
	const PATH_STATIC = program.opts().static;
	const PATH_DATA = process.env.BZD_PATH_DATA || program.opts().data;
	const IS_TEST = Boolean(program.opts().test);
	const AUTHENTICATION_PRIVATE_KEY = process.env.BZD_AUTHENTICATION_PRIVATE_KEY || (IS_TEST ? "dummy" : false);
	const AUTHENTICATION_USERNAME = process.env.BZD_AUTHENTICATION_USERNAME || (IS_TEST ? "admin" : false);
	const AUTHENTICATION_PASSWORD = process.env.BZD_AUTHENTICATION_PASSWORD || (IS_TEST ? "1234" : false);

	Exception.assert(
		AUTHENTICATION_PRIVATE_KEY !== false,
		"A valid authentication private key must be set with the environment variable `BZD_AUTHENTICATION_PRIVATE_KEY`."
	);
	Exception.assert(
		AUTHENTICATION_USERNAME !== false && AUTHENTICATION_PASSWORD !== false,
		"A valid authentication username and password must be set with the environment variable `AUTHENTICATION_USERNAME` and `AUTHENTICATION_PASSWORD`."
	);
	let authentication = new Authentication({
		privateKey: AUTHENTICATION_PRIVATE_KEY,
		verifyIdentityCallback: async (uid, password) => {
			if (uid !== AUTHENTICATION_USERNAME || password !== AUTHENTICATION_PASSWORD) {
				return false;
			}
			return {
				roles: [],
				uid: uid,
			};
		},
		verifyRefreshCallback: async (/*uid, session, timeoutS*/) => {
			return true;
		},
	});

	// Set-up the web server
	let web = new HttpServer(PORT);
	// Install the APIs
	let api = new API(APIv1, {
		authentication: authentication,
		channel: web,
		plugins: [authentication],
	});

	let keyValueStore = await KeyValueStoreDisk.make(Path.join(PATH_DATA, "db"));

	// Test data
	if (IS_TEST) {
		await keyValueStore.set("volume", "disk", {
			type: "fs",
			"fs.root": "/",
		});

		await keyValueStore.set("volume", "docker.blaizard.com", {
			type: "docker",
			"docker.type": "v2",
			"docker.url": "https://docker.blaizard.com",
		});

		await keyValueStore.set("volume", "webdav", {
			type: "webdav",
			"webdav.url": "https://cloud.leviia.com//public.php/webdav",
			"webdav.username": "YsLRkFLLceXbPrB",
		});

		// await keyValueStore.set("volume", "docker.gcr", {
		// type: "docker",
		// "docker.type": "gcr",
		// "docker.key": "",
		// "docker.service": "gcr.io",
		// "docker.url": "https://docker.blaizard.com",
		// "docker.proxy": true,
		// "docker.proxy.url": "http://127.0.0.1:5050",
		// "docker.proxy.port": 5051,
		// });
	}
	// Set the cache
	let cache = new Cache();

	// Retrieve the storage implementation associated with this volume
	cache.register("volume", async (volume) => {
		const params = await keyValueStore.get("volume", volume, null);
		Exception.assert(params !== null, "No volume are associated with this id: '{}'", volume);
		Exception.assert("type" in params, "Unknown type for the volume: '{}'", volume);
		Exception.assert(params.type in Plugins, "Volume type unsupported: '{}'", params.type);
		Exception.assert("storage" in Plugins[params.type], "Storage not supported by plugin '{}'", params.type);
		return await Plugins[params.type].storage(params);
	});

	// Creating services
	let services = new Services({
		makeContext: (volume) => {
			return {
				async getVolume() {
					return await cache.get("volume", volume);
				},
			};
		},
	});

	// Register all plugns
	Log.info("Using plugins: {}", Object.keys(Plugins).join(", "));
	for (const type in Plugins) {
		const pluginServices = Plugins[type].services || {};
		for (const serviceName in pluginServices) {
			services.register(type, serviceName, pluginServices[serviceName]);
		}
	}

	// Start all services
	let it = CollectionPaging.makeIterator(async (maxOrPaging) => {
		return await keyValueStore.list("volume", maxOrPaging);
	}, 50);
	for await (const [name, params] of it) {
		await services.start(name, params.type, params);
	}

	// Redirect /file/** to /file?path=**
	// It is needed to do this before the API as it takes precedence.
	web.addRoute("get", "/file(/*)?", async (request, response) => {
		response.redirect(api.getEndpoint("/file?path=" + request.params[1]));
	});

	// Adding API handlers.
	function getInternalPath(pathList) {
		Exception.assert(Array.isArray(pathList), "Path must be an array: '{:j}'", pathList);
		Exception.assert(
			pathList.every((path) => Boolean(path)),
			"Path elements cannot be empty: '{:j}'",
			pathList
		);
		return { volume: pathList[0], pathList: pathList.slice(1) };
	}

	function getInternalPathFromString(path) {
		return getInternalPath(path.split("/").map(decodeURIComponent));
	}

	api.handle("get", "/file", async function (inputs) {
		const { volume, pathList } = getInternalPathFromString(inputs.path);
		const storage = await cache.get("volume", volume);
		const metadata = await storage.metadata(pathList);
		if (metadata.size) {
			this.setHeader("Content-Length", metadata.size);
		}
		this.setHeader("Content-Disposition", "attachment; filename=\"" + metadata.name + "\"");
		return await storage.read(pathList);
	});

	api.handle("get", "/config", async (inputs) => {
		return await keyValueStore.get("volume", inputs.volume, {});
	});

	api.handle("post", "/config", async (inputs) => {
		// Delete all keys that do not start with <inputs.config.type>.
		Exception.assert("type" in inputs.config, "Configuration type is missing.");
		let params = {
			type: inputs.config.type,
		};
		for (const name in inputs.config) {
			if (name.startsWith(params.type + ".")) {
				params[name] = inputs.config[name];
			}
		}

		// Check if it needs to be renamed
		const volume = inputs.config.volume;
		delete inputs.config.volume;

		// Stop all services related to this config
		await services.stop(inputs.volume);

		await keyValueStore.set("volume", volume, params);
		await cache.setDirty("volume", inputs.volume);
		if (volume != inputs.volume) {
			await keyValueStore.delete("volume", inputs.volume);
		}

		await services.start(volume, params.type, params);
	});

	api.handle("delete", "/config", async (inputs) => {
		await services.stop(inputs.volume);
		return await keyValueStore.delete("volume", inputs.volume);
	});

	api.handle("post", "/list", async (inputs) => {
		const { volume, pathList } = getInternalPath(inputs.path);
		const maxOrPaging = "paging" in inputs ? inputs.paging : 50;

		if (!volume) {
			const volumes = await keyValueStore.list("volume", maxOrPaging);
			const data = Object.entries(volumes.data());
			const result = await CollectionPaging.makeFromList(data, data.length, (item) => {
				return Permissions.makeEntry(
					{
						name: item[0],
						type: "bucket",
						plugin: item[1].type,
					},
					{
						list: true,
					}
				);
			});
			return {
				data: result.data(),
				next: volumes.getNextPaging(),
			};
		}

		const storage = await cache.get("volume", volume);
		const result = await storage.list(pathList, maxOrPaging, /*includeMetadata*/ true);

		return {
			data: result.data(),
			next: result.getNextPaging(),
		};
	});

	api.handle("get", "/services", async (/*inputs*/) => {
		return services.getActive();
	});

	Log.info("Application started");
	web.addStaticRoute("/", PATH_STATIC, "index.html");
	web.start();
})();
