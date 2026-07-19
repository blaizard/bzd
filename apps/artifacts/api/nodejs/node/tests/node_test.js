import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { Node } from "#bzd/apps/artifacts/api/nodejs/node/node.js";

const Exception = ExceptionFactory("test", "artifacts", "node");

/// Build a Node instance with a mocked HTTP client, isolating tests from network and env.
function makeNode({ get = null, post = null } = {}) {
	const httpClient = {
		get: get ?? (async () => ({})),
		post: post ?? (async () => ({})),
	};
	return new Node({
		remotes: ["http://test"],
		volume: "nodes",
		token: null,
		httpClient,
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
});
