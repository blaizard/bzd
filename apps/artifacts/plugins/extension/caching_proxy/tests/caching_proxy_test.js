import ExceptionFactory, { ExceptionPrecondition } from "#bzd/nodejs/core/exception.js";
import Plugin from "#bzd/apps/artifacts/plugins/storage/backend.js";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.js";

const Exception = ExceptionFactory("test", "caching-proxy");

describe("CachingProxy SSRF", () => {
	let tester;

	afterEach(async () => {
		await tester?.stop();
	});

	it("rejects loopback targets", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {},
				write: true,
			},
			cachingProxy: {},
		});
		await tester.start();

		const response = await tester.send(
			"cache",
			"get",
			"/@caching-proxy/http/127.0.0.1:8081/",
			{},
			/*throwOnFailure*/ false,
		);
		Exception.assertEqual(response.status, 400);
	});

	it("rejects unix socket targets", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {},
				write: true,
			},
			cachingProxy: {},
		});
		await tester.start();

		const response = await tester.send(
			"cache",
			"get",
			"/@caching-proxy/unix//var/run/docker.sock",
			{},
			/*throwOnFailure*/ false,
		);
		Exception.assertEqual(response.status, 400);
	});

	it("does not reject allow-listed targets", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {},
				write: true,
			},
			cachingProxy: {
				allowList: ["127.0.0.1:8081"],
			},
		});
		await tester.start();

		// The guard lets the request through; the actual fetch then fails on the
		// network side (nothing listens on the target) but must not be a 400.
		let response = null;
		try {
			response = await tester.send("cache", "get", "/@caching-proxy/http/127.0.0.1:8081/", {}, false);
		} catch (e) {
			Exception.assert(
				!(e instanceof ExceptionPrecondition),
				"The allow-listed target must not be rejected as a precondition: {}",
				e,
			);
		}
		Exception.assert(response === null || response.status !== 400, "The allow-listed target must not return 400.");
	});
});
