import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.js";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.js";
import { timestampMs } from "#bzd/nodejs/utils/timestamp.js";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "nodes");

describe("Nodes", () => {
	describe("getAllPathAndValues", () => {
		it("basic", async () => {
			const result = Nodes.getAllPathAndValues({
				a: {
					b: { c: 12, e: [], f: null },
					d: [12, 3],
				},
			});
			Exception.assertEqual(result, [
				[["a", "b", "c"], 12, {}],
				[["a", "b", "e"], [], {}],
				[["a", "b", "f"], null, {}],
				[["a", "d"], [12, 3], {}],
			]);
		});

		it("string", async () => {
			const result = Nodes.getAllPathAndValues("hello");
			Exception.assertEqual(result, [[[], "hello", {}]]);
		});

		it("array", async () => {
			const result = Nodes.getAllPathAndValues(["a", "b", { c: 1 }]);
			Exception.assertEqual(result, [[[], ["a", "b", { c: 1 }], {}]]);
		});

		it("null", async () => {
			const result = Nodes.getAllPathAndValues(null);
			Exception.assertEqual(result, [[[], null, {}]]);
		});
	});

	describe("Nodes", () => {
		const nodes = new Nodes(/*handlers*/ {}, /*options*/ {});

		it("initial conditions", async () => {
			const result = await nodes.getNodes();
			Exception.assertEqual(result, []);
		});

		it("insert no timestamp", async () => {
			const fragment = await nodes.insert({ uid: "hello", key: ["mykey"], value: "world" });
			Exception.assertEqual(fragment.length, 1);
			Exception.assertEqual(fragment[0].length, 4);
			Exception.assertEqual(fragment[0][0], "hello");
			Exception.assertEqual(fragment[0][1], [["mykey"], "world", {}]);
			Exception.assert(fragment[0][2] <= timestampMs());
			Exception.assertEqual(fragment[0][3], false);
		});

		it("insert w/timestamp (fixed)", async () => {
			const fragment = await nodes.insert({
				uid: "hello",
				key: ["mykey"],
				value: "world",
				timestamp: 123456,
				isFixedTimestamp: true,
			});
			Exception.assertEqual(fragment, [["hello", [["mykey"], "world", {}], 123456, true]]);
		});

		it("insert w/fixed timestamp (non fixed)", async () => {
			const fragment = await nodes.insert({ uid: "hello", key: ["mykey"], value: "world", timestamp: 123456 });
			Exception.assertEqual(fragment, [["hello", [["mykey"], "world", {}], 123456, false]]);
		});
	});
});
