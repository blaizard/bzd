import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Plugin from "#bzd/apps/artifacts/plugins/storage/backend.js";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.js";

const Exception = ExceptionFactory("test", "caching-proxy", "gc");

async function runGC(tester) {
	await tester.serviceRun("cache.cache", "cache.garbage-collect");
	return tester.services.getProcess("cache.cache", "cache.garbage-collect").logs[0].result;
}

describe("CachingProxy garbage collector", () => {
	let tester;

	afterEach(async () => {
		await tester?.stop();
	});

	it("does nothing when under budget", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {
					"metadata.json": JSON.stringify({ "dir/file": 1 }),
					dir: { file: "hello" },
				},
				write: true,
			},
			cachingProxy: { maxStorageSize: 100 },
		});
		await tester.start();

		const result = await runGC(tester);
		Exception.assertEqual(result.filesDeleted, 0);
		Exception.assertEqual(result.totalSize, 5);
	});

	it("evicts oldest file when over budget", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {
					"metadata.json": JSON.stringify({
						"dir/old": 100,
						"dir/new": 200,
					}),
					dir: { old: "aaaa", new: "bbbb" },
				},
				write: true,
			},
			cachingProxy: { maxStorageSize: 5 },
		});
		await tester.start();

		const result = await runGC(tester);
		Exception.assertEqual(result.filesDeleted, 1);
		Exception.assertEqual(result.totalSize, 4);
	});

	it("deletes orphan files not in metadata", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {
					"metadata.json": JSON.stringify({ "dir/tracked": 1 }),
					dir: { tracked: "data", orphan: "zzz" },
				},
				write: true,
			},
			cachingProxy: { maxStorageSize: 1 },
		});
		await tester.start();

		const result = await runGC(tester);
		Exception.assertEqual(result.orphanFilesDeleted, 1);
	});

	it("deletes orphan leaf directories containing no files", async () => {
		tester = new PluginTester();
		tester.register("cache", Plugin, {
			storage: {
				type: "memory",
				data: {
					"metadata.json": JSON.stringify({ "host/real/file": 1 }),
					host: {
						empty: {},
						nested: { empty: {} },
						real: { file: "hello" },
					},
				},
				write: true,
			},
			cachingProxy: { maxStorageSize: 100 },
		});
		await tester.start();

		const result = await runGC(tester);
		Exception.assertEqual(result.orphanDirectoriesDeleted, 3);
		Exception.assertEqual(result.filesDeleted, 0);
	});
});
