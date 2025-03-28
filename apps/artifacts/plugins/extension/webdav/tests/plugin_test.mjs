import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Plugin from "#bzd/apps/artifacts/plugins/storage/backend.mjs";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.mjs";
import { XMLParser } from "fast-xml-parser";

const Exception = ExceptionFactory("test", "artifacts", "extension", "webdav");
const fixedDate = new Date();

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
				options: {
					date: () => fixedDate,
				},
				type: "memory",
			},
			webdav: {},
		});

		it("start", async () => {
			await tester.start();
		});

		/*
		it("propfind (webdav4)", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/");
			Exception.assertEqual(response.status, 207);
			const data = (new XMLParser()).parse(response.data);
			Exception.assertEqual(data.multistatus, 207);
			console.log(data);
		});
		*/

		it("propfind (webdav4)", async () => {
			const response = await tester.send("memory", "propfind", "/webdav/a.txt");
			Exception.assertEqual(response.status, 207);
			const data = new XMLParser().parse(response.data);
			const entry = data.multistatus["D:response"];
			Exception.assertEqual(entry, {
				"D:href": "https://dummy/webdav/a.txt",
				"D:propstat": {
					"D:prop": {
						"D:displayname": "a.txt",
						"D:resourcetype": "",
						"D:getcontentlength": 13,
						"D:getlastmodified": fixedDate.toUTCString(),
						"D:creationdate": fixedDate.toUTCString(),
					},
				},
				"D:status": "HTTP/1.1 200 OK",
			});
		});
	});
});
