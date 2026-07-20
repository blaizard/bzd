import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { Node } from "#bzd/apps/artifacts/api/nodejs/node/node.js";

const Exception = ExceptionFactory("test", "artifacts", "node");

/// Build a Node instance with a mocked HTTP client, isolating tests from network and env.
function makeNode({ get = null, post = null, path = null } = {}) {
	const httpClient = {
		get: get ?? (async () => ({})),
		post: post ?? (async () => ({})),
	};
	return new Node({
		remotes: ["http://test"],
		volume: "nodes",
		token: null,
		httpClient,
		path,
	});
}

describe("Node", () => {
	describe("toTree", () => {
		it("Empty input returns empty object", () => {
			Exception.assertEqual(Node.toTree([]), {});
		});

		it("Single shallow entry", () => {
			Exception.assertEqual(Node.toTree([[["foo"], 42]]), { foo: 42 });
		});

		it("Nested key builds nested object", () => {
			Exception.assertEqual(Node.toTree([[["a", "b", "c"], 1]]), { a: { b: { c: 1 } } });
		});

		it("Same key overwrites keeping latest", () => {
			Exception.assertEqual(
				Node.toTree([
					[["k"], 1],
					[["k"], 2],
				]),
				{ k: 2 },
			);
		});

		it("Mix of paths merges into a shared tree", () => {
			Exception.assertEqual(
				Node.toTree([
					[["a", "x"], 1],
					[["a", "y"], 2],
					[["b"], 3],
				]),
				{ a: { x: 1, y: 2 }, b: 3 },
			);
		});
	});

	describe("get", () => {
		it("Returns the remote result unchanged when metadata is false", async () => {
			const canned = { data: [["k", "v"]], timestamp: 123 };
			const node = makeNode({ get: async () => canned });
			const result = await node.get({ remote: "http://test", uid: "u1" });
			Exception.assertEqual(result, canned);
		});

		it("Preserves the data array shape when metadata is true", async () => {
			const canned = {
				timestamp: 1000,
				data: [["k", [10, "v"]]],
			};
			const node = makeNode({ get: async () => canned });
			const result = await node.get({ remote: "http://test", uid: "u1", metadata: true });

			// Structural-only assertions: the metadata branch reshuffles the first
			// element of each value (t - timestampDiff) but must preserve the
			// outer array length, key, and inner value array length.
			Exception.assert(Array.isArray(result.data), "result.data must remain an array");
			Exception.assertEqual(result.data.length, 1);
			Exception.assertEqual(result.data[0][0], "k");
			Exception.assert(Array.isArray(result.data[0][1]), "inner value must remain an array");
			Exception.assertEqual(result.data[0][1].length, 2);
		});
	});

	describe("getAsTree", () => {
		it("Transforms flat result data into a tree", async () => {
			const canned = {
				timestamp: 0,
				data: [
					[["a", "x"], 1],
					[["a", "y"], 2],
					[["b"], 3],
				],
			};
			const node = makeNode({ get: async () => canned });
			const result = await node.getAsTree({ remote: "http://test", uid: "u1" });
			Exception.assertEqual(result.data, { a: { x: 1, y: 2 }, b: 3 });
		});
	});

	describe("list", () => {
		it("Collects node names from a single page", async () => {
			const node = makeNode({
				post: async () => ({
					data: { n1: { name: "n1" }, n2: { name: "n2" } },
					next: null,
				}),
			});
			const nodes = await node.list({ remote: "http://test" });
			Exception.assertEqual(nodes, ["n1", "n2"]);
		});
	});

	describe("publish", () => {
		it("Posts data with a client timestamp to the data endpoint", async () => {
			const calls = [];
			const node = makeNode({
				post: async (url, options) => {
					calls.push({ url, json: options.json });
					return {};
				},
			});
			await node.publish({ data: "hello", uid: "testuid" });

			Exception.assertEqual(calls.length, 1);
			Exception.assertEqual(calls[0].url, "http://test/x/nodes/testuid/data/");

			const body = calls[0].json;
			Exception.assert(Array.isArray(body.data));
			Exception.assertEqual(body.data[0][1][0][1], "hello");
			Exception.assert(typeof body.timestamp === "number");
		});

		it("Builds a nested URI from the publish path option", async () => {
			const urls = [];
			const node = makeNode({
				post: async (url) => {
					urls.push(url);
					return {};
				},
			});
			await node.publish({ data: 1, uid: "u", path: ["foo", "bar"] });
			Exception.assertEqual(urls[0], "http://test/x/nodes/u/data/foo/bar/");
		});

		it("Prepends the constructor path to the publish path", async () => {
			const urls = [];
			const node = makeNode({
				path: ["root"],
				post: async (url) => {
					urls.push(url);
					return {};
				},
			});
			await node.publish({ data: 1, uid: "u", path: ["foo"] });
			Exception.assertEqual(urls[0], "http://test/x/nodes/u/data/root/foo/");
		});

		it("Throws when posting to all remotes fails", async () => {
			const node = makeNode({
				post: async () => {
					throw new Error("Unreachable");
				},
			});
			await Exception.assertThrows(async () => {
				await node.publish({ data: 1, uid: "testuid" });
			});
		});
	});

	describe("publishBulk", () => {
		it("Posts accumulated entries from the callback to the bulk data endpoint", async () => {
			const calls = [];
			const node = makeNode({
				post: async (url, options) => {
					calls.push({ url, json: options.json });
					return {};
				},
			});
			await node.publishBulk({ uid: "testuid" }, (publish) => {
				publish({ timestampMs: 100, data: "a" });
				publish({ timestampMs: 200, data: "b" });
			});

			Exception.assertEqual(calls.length, 1);
			Exception.assertEqual(calls[0].url, "http://test/x/nodes/testuid/data/");
			Exception.assertEqual(calls[0].json.data, [
				[[], [[100, "a"]]],
				[[], [[200, "b"]]],
			]);
		});

		it("Builds a nested URI from the publishBulk path option", async () => {
			const urls = [];
			const node = makeNode({
				post: async (url) => {
					urls.push(url);
					return {};
				},
			});
			await node.publishBulk({ uid: "u", path: ["foo", "bar"] }, (publish) => {
				publish({ timestampMs: 1, data: "x" });
			});
			Exception.assertEqual(urls[0], "http://test/x/nodes/u/data/foo/bar/");
		});

		it("Prepends the constructor path to the publishBulk path", async () => {
			const urls = [];
			const node = makeNode({
				path: ["root"],
				post: async (url) => {
					urls.push(url);
					return {};
				},
			});
			await node.publishBulk({ uid: "u", path: ["foo"] }, (publish) => {
				publish({ timestampMs: 1, data: "x" });
			});
			Exception.assertEqual(urls[0], "http://test/x/nodes/u/data/root/foo/");
		});

		it("Throws and does not publish when the callback throws", async () => {
			const calls = [];
			const node = makeNode({
				post: async () => {
					calls.push(1);
					return {};
				},
			});
			await Exception.assertThrows(async () => {
				await node.publishBulk({ uid: "u" }, () => {
					throw new Error("boom");
				});
			});
			Exception.assertEqual(calls.length, 0);
		});

		it("Forwards isClientTimestamp false to the underlying _publish call", async () => {
			const calls = [];
			const node = makeNode({
				post: async (url, options) => {
					calls.push({ url, json: options.json });
					return {};
				},
			});
			await node.publishBulk({ uid: "u", isClientTimestamp: false }, (publish) => {
				publish({ timestampMs: 100, data: "x" });
			});

			Exception.assertEqual(calls[0].json.timestamp, undefined);
		});

		it("Forwards default isClientTimestamp true to the underlying _publish call", async () => {
			const calls = [];
			const node = makeNode({
				post: async (url, options) => {
					calls.push({ url, json: options.json });
					return {};
				},
			});
			await node.publishBulk({ uid: "u" }, (publish) => {
				publish({ timestampMs: 100, data: "x" });
			});

			Exception.assert(typeof calls[0].json.timestamp === "number");
		});

		it("Skips the POST when the bulk callback publishes nothing", async () => {
			const calls = [];
			const node = makeNode({
				post: async () => {
					calls.push(1);
					return {};
				},
			});
			await node.publishBulk({ uid: "testuid" }, () => {});
			Exception.assertEqual(calls.length, 0);
		});
	});
});
