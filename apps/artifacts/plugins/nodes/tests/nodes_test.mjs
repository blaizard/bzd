import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Nodes from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "nodes");

describe("Nodes", () => {
	describe("Node", () => {
		it("getAllPathAndValues", async () => {
			const storage = await makeStorageFromConfig({
				type: "memory",
				name: "nodes",
			});
			let nodes = new Nodes(storage, {});
			let node = await nodes.get("node");
			const result = node.getAllPathAndValues({
				a: {
					b: { c: 12, e: [], f: null },
					d: [12, 3],
				},
			});
			Exception.assertEqual(result, {
				"a\u0001b\u0001c": 12,
				"a\u0001b\u0001e": [],
				"a\u0001b\u0001f": null,
				"a\u0001d": [12, 3],
			});
		});
	});
});
