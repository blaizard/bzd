import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Plugin from "#bzd/apps/artifacts/plugins/storage/backend.mjs";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.mjs";
import { XMLParser } from "fast-xml-parser";

const Exception = ExceptionFactory("test", "artifacts", "extension", "webdav");
const fixedDate = new Date();

/// Check that a webdav response contains the expected entries.
const assertEqualResponses = (actual, expected) => {
	try {
		const expectedPrefx = "https://dummy/webdav";
		Exception.assert("multistatus" in actual);
		const responses = Array.isArray(actual.multistatus["D:response"])
			? actual.multistatus["D:response"]
			: [actual.multistatus["D:response"]];
		let consumed = new Set();
		for (const response of responses) {
			Exception.assert(response, "Unexpected response: {:j}", response);
			const href = response["D:href"] || "";
			Exception.assert(
				href.startsWith(expectedPrefx),
				"D:href ({}) must start with '{}': {:j}",
				href,
				expectedPrefx,
				response,
			);
			const path = href.substring(expectedPrefx.length);
			Exception.assert(path in expected, "The path '{}' is not matching any of the expected entries.", path);
			Exception.assert(!(path in consumed), "The path '{}' was covered twice.", path);
			Exception.assertEqual(response["D:status"], "HTTP/1.1 200 OK");
			const props = (response["D:propstat"] ?? {})["D:prop"] ?? {};
			for (const [key, value] of Object.entries(expected[path])) {
				Exception.assert(key in props, "Missing property '{}' in response for '{}', all props {:j}", key, path, props);
				Exception.assertEqual(props[key], value, "Props '{}' for entry '{}', all props {:j}", key, path, props);
			}
		}
	} catch (e) {
		console.error("Received:", JSON.stringify(actual, null, 4));
		throw e;
	}
};

const defaultMemory = {
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
		options: {
			date: () => fixedDate,
		},
		type: "memory",
	},
	webdav: {},
};

describe("Webdav", () => {
	describe("propfind", async () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, defaultMemory);
		await tester.start();

		it("propfind on root directory", async () => {
			const response = await tester.send("memory", "propfind", "/webdav");
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			assertEqualResponses(data, {
				"": {
					"D:displayname": "webdav",
					"D:resourcetype": { "D:collection": "" },
				},
			});
		});

		it("propfind on root directory with '/'", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/");
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			assertEqualResponses(data, {
				"": {
					"D:displayname": "webdav",
					"D:resourcetype": { "D:collection": "" },
				},
			});
		});

		// webdav4: ls
		it("propfind on root directory with depth=1", async () => {
			const response = await tester.send("memory", "propfind", "/webdav", {
				headers: {
					depth: 1,
				},
			});
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			assertEqualResponses(data, {
				"": {
					"D:displayname": "webdav",
					"D:resourcetype": { "D:collection": "" },
				},
				"/a.txt": {
					"D:displayname": "a.txt",
					"D:resourcetype": "",
					"D:getcontentlength": 13,
				},
				"/b.txt": {
					"D:displayname": "b.txt",
					"D:resourcetype": "",
					"D:getcontentlength": 13,
				},
				"/empty": {
					"D:displayname": "empty",
					"D:resourcetype": { "D:collection": "" },
				},
				"/nested": {
					"D:displayname": "nested",
					"D:resourcetype": { "D:collection": "" },
				},
			});
		});

		// webdav4: ls
		it("propfind on nested directory with depth=1", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/nested/deeper", {
				headers: {
					depth: 1,
				},
			});
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			assertEqualResponses(data, {
				"/nested/deeper": {
					"D:displayname": "deeper",
					"D:resourcetype": { "D:collection": "" },
				},
				"/nested/deeper/c.txt": {
					"D:displayname": "c.txt",
					"D:resourcetype": "",
					"D:getcontentlength": 13,
				},
			});
		});

		// webdav4: cat
		it("propfind on file", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/a.txt");
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			const entry = data.multistatus["D:response"];
			assertEqualResponses(data, {
				"/a.txt": {
					"D:displayname": "a.txt",
					"D:resourcetype": "",
					"D:getcontentlength": 13,
				},
			});
		});

		// webdav4: rm file
		it("propfind on file with depth=1", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/a.txt", {
				headers: {
					depth: 1,
				},
			});
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			const entry = data.multistatus["D:response"];
			assertEqualResponses(data, {
				"/a.txt": {
					"D:displayname": "a.txt",
					"D:resourcetype": "",
					"D:getcontentlength": 13,
				},
			});
		});

		it("propfind on wrong path", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/wrong", {}, /*throwOnFailure*/ false);
			Exception.assertEqual(response.status, 404);
		});
	});

	describe("get", async () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, defaultMemory);
		await tester.start();

		it("get file", async () => {
			const response = await tester.send("memory", "get", "/webdav/a.txt");
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.data, "content for a");
		});

		it("get wrong file", async () => {
			const response = await tester.send("memory", "get", "/webdav/wrong.txt", {}, /*throwOnFailure*/ false);
			Exception.assertEqual(response.status, 404);
		});
	});

	describe("put", async () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, defaultMemory);
		await tester.start();

		it("put file", async () => {
			const response = await tester.send("memory", "put", "/webdav/new.txt", {
				data: "hello new content",
			});
			Exception.assertEqual(response.status, 200);

			const responseRead = await tester.send("memory", "get", "/webdav/new.txt");
			Exception.assertEqual(responseRead.status, 200);
			Exception.assertEqual(responseRead.data, "hello new content");
		});

		it("put override file", async () => {
			const response = await tester.send("memory", "put", "/webdav/a.txt", {
				data: "hello new content",
			});
			Exception.assertEqual(response.status, 200);

			const responseRead = await tester.send("memory", "get", "/webdav/a.txt");
			Exception.assertEqual(responseRead.status, 200);
			Exception.assertEqual(responseRead.data, "hello new content");
		});
	});

	describe("delete", async () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, defaultMemory);
		await tester.start();

		it("delete file", async () => {
			const response = await tester.send("memory", "delete", "/webdav/a.txt");
			Exception.assertEqual(response.status, 200);
		});

		it("delete directory", async () => {
			const response = await tester.send("memory", "delete", "/webdav/nested");
			Exception.assertEqual(response.status, 200);
		});

		it("delete wrong resource", async () => {
			const response = await tester.send("memory", "delete", "/webdav/not/exists", {}, /*throwOnFailure*/ false);
			Exception.assertEqual(response.status, 404);
		});
	});

	describe("head", async () => {
		const tester = new PluginTester();
		tester.register("memory", Plugin, defaultMemory);
		await tester.start();

		it("head file", async () => {
			const response = await tester.send("memory", "head", "/webdav/a.txt");
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.headers["Content-Length"], 13);
			Exception.assertEqual(response.headers["Last-Modified"], fixedDate.toUTCString());
			Exception.assertEqual(response.data, null);
		});

		it("head directory", async () => {
			const response = await tester.send("memory", "head", "/webdav/nested");
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.data, null);
		});
	});
});
