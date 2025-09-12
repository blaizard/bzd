import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Nodes, Node } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Utils from "#bzd/apps/artifacts/common/utils.mjs";

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

	describe("Nodes", () => {
		const nodes = new Nodes(/*handlers*/ {}, /*options*/ {});

		it("initial conditions", async () => {
			const result = await nodes.getNodes();
			Exception.assertEqual(result, []);
		});

		it("insert no timestamp", async () => {
			const hello = await nodes.get("hello");
			const fragment = await hello.insert(["mykey"], "world");
			Exception.assertEqual(fragment.length, 1);
			Exception.assertEqual(fragment[0].length, 5);
			Exception.assertEqual(fragment[0][0], "hello");
			Exception.assertEqual(fragment[0][1], ["mykey"]);
			Exception.assertEqual(fragment[0][2], "world");
			Exception.assert(fragment[0][3] <= Utils.timestampMs());
			Exception.assertEqual(fragment[0][4], false);
		});

		it("insert w/timestamp (fixed)", async () => {
			const hello = await nodes.get("hello");
			const fragment = await hello.insert(["mykey"], "world", 123456, /*isFixedTimestamp*/ true);
			Exception.assertEqual(fragment, [["hello", ["mykey"], "world", 123456, true]]);
		});

		it("insert w/fixed timestamp (non fixed)", async () => {
			const hello = await nodes.get("hello");
			const fragment = await hello.insert(["mykey"], "world", 123456);
			Exception.assertEqual(fragment, [["hello", ["mykey"], "world", 123456, false]]);
		});
	});
});
