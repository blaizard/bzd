import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "data");

describe("Nodes", () => {
	describe("Data", () => {
		it("basic", async () => {
			const storage = await makeStorageFromConfig({
				type: "memory",
				name: "nodes",
			});
			const data = new Data(storage.getAccessor("data"));

			// Insert single.
			await data.insert("hello", [[["a", "b"], 12]]);
			// Insert multi.
			await data.insert("hello", [
				[["a", "c", "e"], 1],
				[["a", "c", "d"], 13],
			]);
			// Insert child entry of another leaf.
			await data.insert("hello", [[["a", "b", "d"], -3]]);

			// get w/no options.
			{
				const result = await data.get("hello", ["a", "b"]);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), 12);
			}

			// get wrong key.
			{
				const result1 = await data.get("hello", ["a"]);
				Exception.assert(result1.isEmpty());
				const result2 = await data.get("hello", ["a", "b", "c"]);
				Exception.assert(result2.isEmpty());
				const result3 = await data.get("hello", ["a", "c"]);
				Exception.assert(result3.isEmpty());
				const result4 = await data.get("hello", []);
				Exception.assert(result4.isEmpty());
				const result5 = await data.get("hellop", ["a", "b"]);
				Exception.assert(result5.isEmpty());
			}

			// get w/metadata
			{
				const result = await data.get("hello", ["a", "b"], /*metadata*/ true);
				Exception.assert(result.hasValue());
				Exception.assert(typeof result.value()[0] == "number");
				Exception.assertEqual(result.value()[1], 12);
			}

			// get w/children
			{
				const result = await data.get("hello", ["a", "c"], /*metadata*/ false, /*children*/ true);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["a", "c", "e"], 1],
					[["a", "c", "d"], 13],
				]);
			}

			// get w/children from leaf
			{
				const result = await data.get("hello", ["a", "b"], /*metadata*/ false, /*children*/ true);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["a", "b"], 12],
					[["a", "b", "d"], -3],
				]);
			}
		});

		it("count", async () => {
			const storage = await makeStorageFromConfig({
				type: "memory",
				name: "nodes",
			});
			const data = new Data(storage.getAccessor("data"));

			await data.insert("hello", [[["a", "b"], 1]]);
			await data.insert("hello", [[["a", "b"], 2]]);

			// get w/no options.
			{
				const result = await data.get("hello", ["a", "b"]);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), 2);
			}

			// count of 1 should return an array
			{
				const result = await data.get("hello", ["a", "b"], /*metadata*/ false, /*children*/ false, /*count*/ 1);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2]);
			}

			{
				const result = await data.get("hello", ["a", "b"], /*metadata*/ false, /*children*/ false, /*count*/ 2);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// count greater than data
			{
				const result = await data.get("hello", ["a", "b"], /*metadata*/ false, /*children*/ false, /*count*/ 3);
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// wrong key
			{
				const result = await data.get("hello", ["a", "c"], /*metadata*/ false, /*children*/ false, /*count*/ 2);
				Exception.assert(result.isEmpty());
			}
		});
	});
});
