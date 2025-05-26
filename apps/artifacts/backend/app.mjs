import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import ServiceProvider from "#bzd/nodejs/core/services/provider.mjs";
import EndpointsFactory from "#bzd/apps/artifacts/backend/endpoints_factory.mjs";
import Plugin from "#bzd/apps/artifacts/backend/plugin.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Plugins from "#bzd/apps/artifacts/plugins/backend.mjs";
import config from "#bzd/apps/artifacts/backend/config.json" with { type: "json" };
import { FileNotFoundError } from "#bzd/nodejs/db/storage/storage.mjs";

import Backend from "#bzd/nodejs/vue/apps/backend.mjs";

const Log = LogFactory("backend");
const Exception = ExceptionFactory("backend");

// For debugging purposes.
// Log.config.console.setLevel("debug");

(async () => {
	const volumes = {};

	// Backend
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useServices()
		.useCache()
		.useStatistics()
		.setup();

	for (const [token, options] of Object.entries(config["tokens"] || {})) {
		await backend.authentication.preloadApplicationToken(token, options["scopes"]);
	}
	Log.info("Preloaded {} application token(s).", Object.keys(config["tokens"] || {}).length);

	// Add initial volumes.
	for (const [volume, options] of Object.entries(config.volumes)) {
		Exception.assert("type" in options, "The volume '{}' must have a 'type'.", volume);
		const type = options.type;
		Exception.assert(type in Plugins, "No plugins of type '{}', requested by '{}'.", type, volume);
		Exception.assert(Plugins[type].prototype instanceof Plugin, "The plugin '{}' must derive from class Plugin.", type);

		const provider = new ServiceProvider(volume);
		const endpoints = new EndpointsFactory();
		const instance = new Plugins[type](volume, options, provider, endpoints);
		provider.addStopProcess(() => {
			// This stop process will be executed at last.
			backend.cache.setDirty("volume", volume);
			instance.resetStorage();
		});
		const serviceId = backend.services.register(provider, type);

		volumes[volume] = {
			options: options,
			instance: instance,
			endpoints: endpoints.unwrap(),
			serviceId: serviceId,
		};
	}

	// Retrieve the storage implementation associated with this volume
	backend.cache.register("volume", async (volume) => {
		Exception.assert(volume in volumes, "No volume are associated with this id: '{}'", volume);
		const instance = volumes[volume].instance;
		return instance.getStorage();
	});

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

	// Redirect /file/** to /file?path=**
	// It is needed to do this before the REST API as it takes precedence.
	backend.web.addRoute("get", "/file/{path:*}", async (context) => {
		context.redirect(backend.rest.getEndpoint("/file?path=" + context.getParam("path")));
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

	async function isAuthorizedVolume(context, volume) {
		if (!(volume in volumes)) {
			return false;
		}
		if ("private" in volumes[volume].options) {
			const maybeSession = await backend.authentication.verify(context, /*scopes*/ [volume]);
			if (!maybeSession) {
				return false;
			}
		}
		return true;
	}

	async function assertAuthorizedVolume(context, volume) {
		if (!(await isAuthorizedVolume(context, volume))) {
			throw backend.authentication.httpErrorUnauthorized(/*requestAuthentication*/ true);
		}
	}

	function assertVolumeReady(volume) {
		const serviceId = volumes[volume].serviceId;
		const state = backend.services.getService(serviceId);
		Exception.assertPrecondition(state.status == "running", "Volume '{}' is not ready. {}", volume, state.toString());
	}

	backend.rest.handle(
		"get",
		"/file",
		async function (inputs) {
			try {
				const { volume, pathList } = getInternalPathFromString(inputs.path);

				assertVolumeReady(volume);
				await assertAuthorizedVolume(this, volume);

				Exception.assertPrecondition(volume, "There is no volume associated with this path: '{}'.", inputs.path);
				const storage = await backend.cache.get("volume", volume);
				const metadata = await storage.metadata(pathList);
				if (metadata.size) {
					this.setHeader("Content-Length", metadata.size);
				}
				this.setHeader("Content-Disposition", 'attachment; filename="' + metadata.name + '"');
				return await storage.read(pathList);
			} catch (e) {
				if (e instanceof FileNotFoundError) {
					throw this.httpError(404, "Not Found");
				}
				throw e;
			}
		},
		{
			timeoutS: 10 * 60, // 10min timeout
		},
	);

	backend.rest.handle("post", "/list", async function (inputs) {
		try {
			const { volume, pathList } = getInternalPath(inputs.path);
			const maxOrPaging = "paging" in inputs ? inputs.paging : 50;

			if (!volume) {
				const volumeNames = Object.keys(volumes);
				const result = await CollectionPaging.makeFromList(volumeNames, volumeNames.length, async (volume) => {
					return Permissions.makeEntry(
						{
							name: volume,
							type: "bucket",
							plugin: volumes[volume].options.type,
							authorized: await isAuthorizedVolume(this, volume),
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

			assertVolumeReady(volume);
			await assertAuthorizedVolume(this, volume);

			const storage = await backend.cache.get("volume", volume);
			const result = await storage.list(pathList, maxOrPaging, /*includeMetadata*/ true);

			return {
				data: result.data(),
				next: result.getNextPaging(),
			};
		} catch (e) {
			if (e instanceof FileNotFoundError) {
				throw this.httpError(404, "Not Found");
			}
			throw e;
		}
	});

	for (const [volume, data] of Object.entries(volumes)) {
		for (const [method, endpoints] of Object.entries(data.endpoints)) {
			for (const endpoint of endpoints) {
				const route = "/x/" + volume + endpoint.path;
				Log.info("Adding route {}::{}", method, route);
				backend.web.addRoute(
					method,
					route,
					async (context) => {
						assertVolumeReady(volume);
						await assertAuthorizedVolume(context, volume);
						return await endpoint.handler(context);
					},
					Object.assign({ exceptionGuard: true, type: ["raw"] }, endpoint.options),
				);
			}
		}
	}
	// Catch all to return 404 in case a wrong path is given.
	backend.web.addRoute("*", "/x/{all:*}", (context) => context.sendStatus(404, "File Not Found"));
	await backend.start();

	if (backend.test) {
		await backend.web.test(config.tests || []);
		await backend.stop();
	}
})();
