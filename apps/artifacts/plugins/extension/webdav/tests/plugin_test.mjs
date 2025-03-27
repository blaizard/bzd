import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Plugin from "#bzd/apps/artifacts/plugins/storage/backend.mjs";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.mjs";

const Exception = ExceptionFactory("test", "artifacts", "extension", "webdav");

describe("Webdav", () => {
	describe("Plugin", () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, {
			storage: {
				data: {
					"a.txt": "content for a",
					"b.txt": "content for b",
					empty: {},
					nested: {
						deeper: {
							"c.txt": "content for c",
						},
						"e.txt": "content for e",
						"f.txt": "content for b",
					},
				},
				type: "memory",
			},
			webdav: {},
		});

		it("start", async () => {
			await tester.start();
		});

		it("propfind (webdav4)", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/a.txt");
			Exception.assertEqual(response.status, 207);
		});
	});
});
