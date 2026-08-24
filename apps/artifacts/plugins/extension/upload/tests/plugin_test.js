import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Plugin from "#bzd/apps/artifacts/plugins/storage/backend.js";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.js";

const Exception = ExceptionFactory("test", "artifacts", "extension", "upload");
const fixedDate = new Date();

const defaultMemory = {
	storage: {
		data: {
			apps: {
				node_manager: {
					"linux-x86_64": {},
				},
			},
		},
		options: {
			date: () => fixedDate,
		},
		write: true,
		type: "memory",
	},
	upload: {
		"/apps": {
			limit: 1073741824,
			rolling: 5,
		},
	},
};

describe("Upload", () => {
	describe("put", async () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, defaultMemory);
		await tester.start();
		const storage = tester.plugins["memory"].instance.getStorage();

		it("put file in a nested path", async () => {
			const response = await tester.send("memory", "put", "/apps/node_manager/linux-x86_64/release-node_manager", {
				data: "content for release",
			});
			Exception.assertEqual(response.status, 200);

			const content = await storage.readToString(["apps", "node_manager", "linux-x86_64", "release-node_manager"]);
			Exception.assertEqual(content, "content for release");
		}, 10000);

		it("put file keeps only the last files", async () => {
			await tester.send("memory", "put", "/apps/node_manager/linux-x86_64/release-1", { data: "content 1" });
			await tester.send("memory", "put", "/apps/node_manager/linux-x86_64/release-2", { data: "content 2" });
			await tester.send("memory", "put", "/apps/node_manager/linux-x86_64/release-3", { data: "content 3" });

			const files = await storage.listAll(["apps", "node_manager", "linux-x86_64"]);
			Exception.assertEqual(files.sort(), ["release-1", "release-2", "release-3", "release-node_manager"]);
		}, 10000);
	});
});
