import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Nodes from "#bzd/apps/artifacts/plugins/bzd/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "bzd");

describe("Nodes", () => {
	describe("Node", () => {
		it("getAllPathAndValues", async () => {
			const storage = await makeStorageFromConfig({
				type: "memory",
				name: "nodes",
			});
			let nodes = new Nodes(storage);
			let node = await nodes.get("node");
			const result = node.getAllPathAndValues({
				a: {
					b: { c: 12, e: [], f: null },
					d: [12, 3],
				},
			});
			Exception.assertEqual(result, { "a.b.c": 12, "a.b.e": [], "a.b.f": null, "a.d": [12, 3] });
		});
	});
});
