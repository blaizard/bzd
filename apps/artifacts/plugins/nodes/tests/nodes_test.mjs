import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Nodes, Node } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
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
				[Node.keyToInternal("a", "b", "c")]: 12,
				[Node.keyToInternal("a", "b", "e")]: [],
				[Node.keyToInternal("a", "b", "f")]: null,
				[Node.keyToInternal("a", "d")]: [12, 3],
			});
		});
	});
});
