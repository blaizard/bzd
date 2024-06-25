import RestServer from "#bzd/nodejs/core/rest/server.mjs";

import Cache from "#bzd/nodejs/core/cache.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import { Command } from "commander/esm.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";
import ServiceProvider from "#bzd/nodejs/core/services/provider.mjs";
import Storage from "#bzd/nodejs/db/storage/storage.mjs";
import EndpointsFactory from "#bzd/apps/artifacts/backend/endpoints_factory.mjs";
import Authentication from "#bzd/apps/accounts/authentication/server.mjs";

import APIv1 from "#bzd/api.json" assert { type: "json" };
import Plugins from "#bzd/apps/artifacts/plugins/backend.mjs";
import config from "#bzd/apps/artifacts/backend/config.json" assert { type: "json" };
import configGlobal from "#bzd/apps/artifacts/config.json" assert { type: "json" };

const Log = LogFactory("backend");
const Exception = ExceptionFactory("backend");

// For debugging purposes.
// Log.config.console.setLevel("debug");

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with the environment variable BZD_PORT.",
		8080,
		parseInt,
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.parse(process.argv);

(async () => {
	// Read arguments
	const PORT = Number(process.env.BZD_PORT || program.opts().port);
	const PATH_STATIC = program.opts().static;

	const volumes = {};

	// Set the cache
	const cache = new Cache();

	// Services
	const services = new Services();

	const authentication = new Authentication({
		accounts: configGlobal.accounts,
	});

	// Set-up the web server
	let web = new HttpServer(PORT);
	let rest = new RestServer(APIv1.rest, {
		authentication: authentication,
		channel: web,
	});
	await rest.installPlugins(authentication, services);

	// Add initial volumes.
	for (const [volume, options] of Object.entries(config.volumes)) {
		Exception.assert("type" in options, "Volume options must have a type: '{}'.", volume);
		const type = options.type;
		Exception.assert(type in Plugins, "No plugins of type '{}', requested by '{}'.", type, volume);

		const provider = new ServiceProvider(volume);
		provider.addStopProcess(() => {
			cache.setDirty("volume", volume);
		});
		const endpoints = new EndpointsFactory();
		const instance = new Plugins[type](volume, options, provider, endpoints);
		services.register(provider, type);

		volumes[volume] = {
			options: options,
			instance: instance,
			endpoints: endpoints.unwrap(),
		};
	}

	// Retrieve the storage implementation associated with this volume
	cache.register("volume", async (volume) => {
		Exception.assert(volume in volumes, "No volume are associated with this id: '{}'", volume);
		const instance = volumes[volume].instance;
		Exception.assert(instance.storage, "No storage associated with this volume: '{}'", volume);
		Exception.assert(
			instance.storage instanceof Storage,
			"The storage class of volume '{}' must be of type Storage.",
			volume,
		);
		return instance.storage;
	});

	// Start all the services.
	await services.start();

	// Docker GCR example
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

	const endpointHandler = async function (method, volume, pathList) {
		Exception.assertPrecondition(volume, "There is no volume associated with this path: '{}'.", pathList.join("/"));
		Exception.assertPrecondition(volume in volumes, "No volume are associated with this id: '{}'", volume);
		const endpoints = volumes[volume].endpoints;

		// Look for a match.
		const regexprs = endpoints[method];
		const path = pathList.map(encodeURIComponent).join("/");
		for (const data of regexprs) {
			const match = data.regexpr.exec(path);
			if (match) {
				const values = Object.assign({}, match.groups);
				return await data.handler.call(this, values);
			}
		}

		Exception.assertPrecondition(false, "Unhandled endpoint for /{}", [volume, ...pathList].join("/"));
	};

	// Redirect /file/** to /file?path=**
	// It is needed to do this before the REST API as it takes precedence.
	web.addRoute("get", "/file/{path:*}", async (context) => {
		context.redirect(rest.getEndpoint("/file?path=" + context.getParam("path")));
	});

	// Adding REST handlers.
	function getInternalPath(pathList) {
		Exception.assert(Array.isArray(pathList), "Path must be an array: '{:j}'", pathList);
		pathList = pathList.filter((path) => Boolean(path));
		return { volume: pathList[0], pathList: pathList.slice(1) };
	}

	function getInternalPathFromString(path) {
		return getInternalPath(path.split("/").map(decodeURIComponent));
	}

	rest.handle("get", "/file", async function (inputs) {
		const { volume, pathList } = getInternalPathFromString(inputs.path);
		Exception.assertPrecondition(volume, "There is no volume associated with this path: '{}'.", inputs.path);
		const storage = await cache.get("volume", volume);
		const metadata = await storage.metadata(pathList);
		if (metadata.size) {
			this.setHeader("Content-Length", metadata.size);
		}
		this.setHeader("Content-Disposition", 'attachment; filename="' + metadata.name + '"');
		return await storage.read(pathList);
	});

	rest.handle("post", "/list", async (inputs) => {
		const { volume, pathList } = getInternalPath(inputs.path);
		const maxOrPaging = "paging" in inputs ? inputs.paging : 50;

		if (!volume) {
			const volumeNames = Object.keys(volumes);
			const result = await CollectionPaging.makeFromList(volumeNames, volumeNames.length, (volume) => {
				return Permissions.makeEntry(
					{
						name: volume,
						type: "bucket",
						plugin: volumes[volume].options.type,
					},
					{
						list: true,
					},
				);
			});
			return {
				data: result.data(),
				next: null,
			};
		}

		const storage = await cache.get("volume", volume);
		const result = await storage.list(pathList, maxOrPaging, /*includeMetadata*/ true);

		return {
			data: result.data(),
			next: result.getNextPaging(),
		};
	});

	for (const method of ["get", "post"]) {
		web.addRoute(
			method,
			"/x/{path:*}",
			async (context) => {
				const path = context.getParam("path");
				const { volume, pathList } = getInternalPathFromString(path);
				if (method == "get") {
					const data = await endpointHandler.call(context, method, volume, pathList);
					context.sendJson(data);
				} else {
					await endpointHandler.call(context, method, volume, pathList);
					context.sendStatus(200);
				}
			},
			{ exceptionGuard: true, type: ["raw"] },
		);
	}

	Log.info("Application started");
	web.addStaticRoute("/", PATH_STATIC);
	web.start();
})();
