import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Nodes, Node } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "nodes");

describe("Nodes", () => {
	describe("Node", () => {
		it("getAllPathAndValues", async () => {
			const result = Node.getAllPathAndValues({
				a: {
					b: { c: 12, e: [], f: null },
					d: [12, 3],
				},
			});
			Exception.assertEqual(result, [
				[["a", "b", "c"], 12],
				[["a", "b", "e"], []],
				[["a", "b", "f"], null],
				[
					["a", "d"],
					[12, 3],
				],
			]);
		});

		it("string", async () => {
			const result = Node.getAllPathAndValues("hello");
			Exception.assertEqual(result, [[[], "hello"]]);
		});

		it("array", async () => {
			const result = Node.getAllPathAndValues(["a", "b", { c: 1 }]);
			Exception.assertEqual(result, [[[], ["a", "b", { c: 1 }]]]);
		});

		it("null", async () => {
			const result = Node.getAllPathAndValues(null);
			Exception.assertEqual(result, [[[], null]]);
		});
	});
});
