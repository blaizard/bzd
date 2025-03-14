import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Plugin from "#bzd/apps/artifacts/plugins/nodes/backend.mjs";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "nodes");

describe("Nodes", () => {
	describe("Plugin", () => {
		const tester = new PluginTester();
		tester.register("nodes", Plugin, {
			"nodes.records": {
				path: "./records",
			},
		});

		it("start", async () => {
			await tester.start();
		});

		it("read empty", async () => {
			Exception.assertThrows(async () => {
				await tester.send("nodes", "get", "/uid01/unknown/path");
			});
		});

		it("write string", async () => {
			await tester.send("nodes", "post", "/uid01/hello/string", {
				headers: { "Content-Type": "text/plain" },
				data: "hello",
			});
			const response = await tester.send("nodes", "get", "/uid01/hello/string");
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.data, { data: "hello" });
		});

		it("write dictionary", async () => {
			await tester.send("nodes", "post", "/uid01/hello/dict", {
				headers: { "Content-Type": "application/json" },
				data: JSON.stringify({ a: 1, b: 2 }),
			});
			Exception.assertThrows(async () => {
				// not found
				await tester.send("nodes", "get", "/uid01/hello/dict");
			});
			{
				const response = await tester.send("nodes", "get", "/uid01/hello/dict/a");
				Exception.assertEqual(response.status, 200);
				Exception.assertEqual(response.data, { data: 1 });
			}
			{
				const response = await tester.send("nodes", "get", "/uid01/hello/dict?children=10");
				Exception.assertEqual(response.status, 200);
				Exception.assertEqual(response.data, {
					data: [
						[["a"], 1],
						[["b"], 2],
					],
				});
			}
		});

		it("write malformed", async () => {
			const response = await tester.send(
				"nodes",
				"post",
				"/uid01/hello/malformed",
				{
					headers: { "Content-Type": "application/json" },
					data: "{not well formed",
				},
				/*throwOnFailure*/ false,
			);
			Exception.assertEqual(response.status, 400);
		});

		it("read malformed", async () => {
			const response = await tester.send(
				"nodes",
				"get",
				"/uid01/hello/dict?children=dsdd",
				{},
				/*throwOnFailure*/ false,
			);
			Exception.assert(response.status != 200);
		});

		it("stop", async () => {
			await tester.stop();
		});
	});

	describe("Plugin reload a second time", () => {
		const tester = new PluginTester();
		tester.register("nodes", Plugin, {
			"nodes.records": {
				path: "./records",
			},
		});

		it("start", async () => {
			await tester.start();
		});

		it("read previous entry", async () => {
			const response = await tester.send("nodes", "get", "/uid01/hello/string");
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.data, { data: "hello" });
		});

		it("stop", async () => {
			await tester.stop();
		});
	});
});
