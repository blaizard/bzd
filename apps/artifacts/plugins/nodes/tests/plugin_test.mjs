import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Plugin from "#bzd/apps/artifacts/plugins/nodes/backend.mjs";
import PluginTester from "#bzd/apps/artifacts/backend/plugin_tester.mjs";
import { makeMockHttpClientFactory } from "#bzd/nodejs/core/http/mock/client.mjs";
import { delayMs } from "#bzd/nodejs/utils/delay.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "nodes");

const waitUntil = async (callback) => {
	while (true) {
		if (await callback()) {
			return;
		}
		await delayMs(10);
	}
};

describe("Nodes", () => {
	describe("Plugin", () => {
		const tester = new PluginTester();
		tester.register("nodes", Plugin, {
			"nodes.records": {
				path: "./records",
				clean: true,
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

		it("read @records", async () => {
			const response = await tester.send("nodes", "get", "/@records");
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.data.records.length, 2);
			Exception.assertEqual(response.data.end, true);
			Exception.assertEqual(response.data.next, 3);
			Exception.assertEqual(response.data.version, 2);
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

	describe("RecordTo/FromDisk", () => {
		const recordTest1 = [
			["udm", ["data", "disk", "/dev/loop0"], [741081088, 741081088], 1742396372419],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/overlay"], [1492955136, 9898135552], 1742396372419],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/log"], [227487744, 1020702720], 1742396372419],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/persistent"], [445366272, 2040373248], 1742396372419],
			["udm", ["data", "disk", "/dev/sda5"], [1062014976, 116971257856], 1742396372419],
			["udm", ["data", "uptime"], 11829212.253938198, 1742396372419],
			["accounts", ["data", "cpu", "main"], [0.01], 1742396377036],
			["accounts", ["data", "memory", "ram"], [624484352, 1003941888], 1742396377036],
			["accounts", ["data", "disk", "/dev/vda1"], [14162665472, 25821052928], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop2"], [109051904, 109051904], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop0"], [4456448, 4456448], 1742396377036],
			["accounts", ["data", "disk", "/dev/vda15"], [6333952, 109422592], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop8"], [109314048, 109314048], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop9"], [91357184, 91357184], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop10"], [4718592, 4718592], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop3"], [66846720, 66846720], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop13"], [93847552, 93847552], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop5"], [77463552, 77463552], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop4"], [46661632, 46661632], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop6"], [77594624, 77594624], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop11"], [66846720, 66846720], 1742396377036],
			["accounts", ["data", "disk", "/dev/loop7"], [46661632, 46661632], 1742396377036],
			["accounts", ["data", "uptime"], 23262548.98503518, 1742396377036],
			["udm", ["data", "cpu", "main"], [0.052000000000000005, 0.031, 0.04, 0.10800000000000001], 1742396378612],
			["udm", ["data", "temperature", "adt7475"], [52.25, 50.75, 52.25], 1742396378612],
			["udm", ["data", "memory", "ram"], [2608898048, 4137803776], 1742396378612],
			["udm", ["data", "memory", "swap"], [575930368, 7516188672], 1742396378612],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/root"], [1475219456, 2040373248], 1742396378612],
			["udm", ["data", "disk", "/dev/loop0"], [741081088, 741081088], 1742396378612],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/overlay"], [1492955136, 9898135552], 1742396378612],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/log"], [227487744, 1020702720], 1742396378612],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/persistent"], [445366272, 2040373248], 1742396378612],
			["udm", ["data", "disk", "/dev/sda5"], [1062014976, 116971257856], 1742396378612],
			["udm", ["data", "uptime"], 11829218.455123663, 1742396378612],
			["accounts", ["data", "cpu", "main"], [0.01], 1742396383089],
			["accounts", ["data", "memory", "ram"], [635863040, 1003941888], 1742396383089],
			["accounts", ["data", "disk", "/dev/vda1"], [14162665472, 25821052928], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop2"], [109051904, 109051904], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop0"], [4456448, 4456448], 1742396383089],
			["accounts", ["data", "disk", "/dev/vda15"], [6333952, 109422592], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop8"], [109314048, 109314048], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop9"], [91357184, 91357184], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop10"], [4718592, 4718592], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop3"], [66846720, 66846720], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop13"], [93847552, 93847552], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop5"], [77463552, 77463552], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop4"], [46661632, 46661632], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop6"], [77594624, 77594624], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop11"], [66846720, 66846720], 1742396383089],
			["accounts", ["data", "disk", "/dev/loop7"], [46661632, 46661632], 1742396383089],
			["accounts", ["data", "uptime"], 23262555.03905797, 1742396383089],
			["udm", ["data", "cpu", "main"], [0.14, 0.156, 0.05, 0.214], 1742396384824],
			["udm", ["data", "temperature", "adt7475"], [52.25, 50.75, 52.5], 1742396384824],
			["udm", ["data", "memory", "ram"], [2609442816, 4137803776], 1742396384824],
			["udm", ["data", "memory", "swap"], [575930368, 7516188672], 1742396384824],
			["udm", ["data", "disk", "/dev/disk/by-partlabel/root"], [1475219456, 2040373248], 1742396384824],
		];

		const recordOnDiskTest1 = [
			[
				"udm",
				{
					data: {
						disk: {
							"/dev/loop0": { _: [741081088, 741081088] },
							"/dev/disk/by-partlabel/overlay": { _: [1492955136, 9898135552] },
							"/dev/disk/by-partlabel/log": { _: [227487744, 1020702720] },
							"/dev/disk/by-partlabel/persistent": { _: [445366272, 2040373248] },
							"/dev/sda5": { _: [1062014976, 116971257856] },
						},
						uptime: { _: 11829212.253938198 },
					},
				},
				1742396372419,
			],
			[
				"accounts",
				{
					data: {
						cpu: { main: { _: [0.01] } },
						memory: { ram: { _: [624484352, 1003941888] } },
						disk: {
							"/dev/vda1": { _: [14162665472, 25821052928] },
							"/dev/loop2": { _: [109051904, 109051904] },
							"/dev/loop0": { _: [4456448, 4456448] },
							"/dev/vda15": { _: [6333952, 109422592] },
							"/dev/loop8": { _: [109314048, 109314048] },
							"/dev/loop9": { _: [91357184, 91357184] },
							"/dev/loop10": { _: [4718592, 4718592] },
							"/dev/loop3": { _: [66846720, 66846720] },
							"/dev/loop13": { _: [93847552, 93847552] },
							"/dev/loop5": { _: [77463552, 77463552] },
							"/dev/loop4": { _: [46661632, 46661632] },
							"/dev/loop6": { _: [77594624, 77594624] },
							"/dev/loop11": { _: [66846720, 66846720] },
							"/dev/loop7": { _: [46661632, 46661632] },
						},
						uptime: { _: 23262548.98503518 },
					},
				},
				1742396377036,
			],
			[
				"udm",
				{
					data: {
						cpu: { main: { _: [0.052000000000000005, 0.031, 0.04, 0.10800000000000001] } },
						temperature: { adt7475: { _: [52.25, 50.75, 52.25] } },
						memory: { ram: { _: [2608898048, 4137803776] }, swap: { _: [575930368, 7516188672] } },
						disk: {
							"/dev/disk/by-partlabel/root": { _: [1475219456, 2040373248] },
							"/dev/loop0": { _: [741081088, 741081088] },
							"/dev/disk/by-partlabel/overlay": { _: [1492955136, 9898135552] },
							"/dev/disk/by-partlabel/log": { _: [227487744, 1020702720] },
							"/dev/disk/by-partlabel/persistent": { _: [445366272, 2040373248] },
							"/dev/sda5": { _: [1062014976, 116971257856] },
						},
						uptime: { _: 11829218.455123663 },
					},
				},
				1742396378612,
			],
			[
				"accounts",
				{
					data: {
						cpu: { main: { _: [0.01] } },
						memory: { ram: { _: [635863040, 1003941888] } },
						disk: {
							"/dev/vda1": { _: [14162665472, 25821052928] },
							"/dev/loop2": { _: [109051904, 109051904] },
							"/dev/loop0": { _: [4456448, 4456448] },
							"/dev/vda15": { _: [6333952, 109422592] },
							"/dev/loop8": { _: [109314048, 109314048] },
							"/dev/loop9": { _: [91357184, 91357184] },
							"/dev/loop10": { _: [4718592, 4718592] },
							"/dev/loop3": { _: [66846720, 66846720] },
							"/dev/loop13": { _: [93847552, 93847552] },
							"/dev/loop5": { _: [77463552, 77463552] },
							"/dev/loop4": { _: [46661632, 46661632] },
							"/dev/loop6": { _: [77594624, 77594624] },
							"/dev/loop11": { _: [66846720, 66846720] },
							"/dev/loop7": { _: [46661632, 46661632] },
						},
						uptime: { _: 23262555.03905797 },
					},
				},
				1742396383089,
			],
			[
				"udm",
				{
					data: {
						cpu: { main: { _: [0.14, 0.156, 0.05, 0.214] } },
						temperature: { adt7475: { _: [52.25, 50.75, 52.5] } },
						memory: { ram: { _: [2609442816, 4137803776] }, swap: { _: [575930368, 7516188672] } },
						disk: { "/dev/disk/by-partlabel/root": { _: [1475219456, 2040373248] } },
					},
				},
				1742396384824,
			],
		];

		it("RecordToDisk", async () => {
			const result = Plugin.recordToDisk(recordTest1);
			Exception.assertEqual(result, recordOnDiskTest1);
			Exception.assertEqual(JSON.stringify(recordTest1).length, 4290);
			Exception.assertEqual(JSON.stringify(result).length, 2662);
		});

		it("RecordFromDisk", async () => {
			const result = Plugin.recordFromDisk(recordOnDiskTest1);
			Exception.assertEqual(result, recordTest1);
		});
	});

	const makeRemoteTest = async (onFetchCallback, verify) => {
		const tester = new PluginTester();
		let fetched = false;
		tester.register(
			"nodes",
			Plugin,
			{
				"nodes.records": {
					path: "./records",
					clean: true,
				},
				"nodes.remotes": {
					remote1: {
						host: "http://remote1",
						delayS: 0.1,
					},
				},
			},
			{
				HttpClientFactory: makeMockHttpClientFactory((url, options) => {
					fetched = true;
					return onFetchCallback(url, options);
				}),
			},
		);

		await tester.start();

		try {
			await waitUntil(() => fetched);
			const response = await tester.send("nodes", "get", "/@records");
			Exception.assertEqual(response.status, 200);
			verify(response.data.records);
		} finally {
			await tester.stop();
		}
	};

	describe("Remote", () => {
		it("empty", async () => {
			await makeRemoteTest(
				() => ({
					version: Plugin.version,
					timestamp: 121231,
					records: [],
					next: 1,
					end: true,
				}),
				(records) => {
					Exception.assertEqual(records, []);
				},
			);
		});
	});
});
