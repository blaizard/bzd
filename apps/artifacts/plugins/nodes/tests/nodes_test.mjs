import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { Nodes, Node } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";

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
			const result = Node.getAllPathAndValues({
				a: {
					b: { c: 12, e: [], f: null },
					d: [12, 3],
				},
			});
			Exception.assertEqual(result, {
				[KeyMapping.keyToInternal(["a", "b", "c"])]: 12,
				[KeyMapping.keyToInternal(["a", "b", "e"])]: [],
				[KeyMapping.keyToInternal(["a", "b", "f"])]: null,
				[KeyMapping.keyToInternal(["a", "d"])]: [12, 3],
			});
		});
	});
});
