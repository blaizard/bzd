import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "data");

describe("Nodes", () => {
	describe("Data", () => {
		it("basic", async () => {
			const data = new Data();

			// Insert single.
			await data.insert("hello", [[["a", "b"], 12]]);
			// Insert multi.
			await data.insert("hello", [
				[["a", "c", "e"], 1],
				[["a", "c", "d"], 13],
				[["a", "c", "d", "f"], 4],
			]);
			// Insert child entry of another leaf.
			await data.insert("hello", [[["a", "b", "d"], -3]]);

			// get w/no options.
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"] });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), 12);
			}

			// get wrong key.
			{
				const result1 = await data.get({ uid: "hello", key: ["a"] });
				Exception.assert(result1.isEmpty());
				const result2 = await data.get({ uid: "hello", key: ["a", "b", "c"] });
				Exception.assert(result2.isEmpty());
				const result3 = await data.get({ uid: "hello", key: ["a", "c"] });
				Exception.assert(result3.isEmpty());
				const result4 = await data.get({ uid: "hello", key: [] });
				Exception.assert(result4.isEmpty());
				const result5 = await data.get({ uid: "hellop", key: ["a", "b"] });
				Exception.assert(result5.isEmpty());
			}

			// get w/metadata
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], metadata: true });
				Exception.assert(result.hasValue());
				Exception.assert(typeof result.value()[0] == "number");
				Exception.assertEqual(result.value()[1], 12);
			}

			// get w/children
			{
				const result = await data.get({ uid: "hello", key: ["a", "c"], children: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["e"], 1],
					[["d"], 13],
				]);
			}

			// get w/children with level 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "c"], children: 2 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["e"], 1],
					[["d"], 13],
					[["d", "f"], 4],
				]);
			}

			// get w/children from leaf
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], children: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[[], 12],
					[["d"], -3],
				]);
			}

			// get w/children wrong key
			{
				const result = await data.get({ uid: "hello", key: ["a", "u"], children: 1 });
				Exception.assert(!result.hasValue());
			}

			// get w/children wrong key '_'
			{
				const result = await data.get({ uid: "hello", key: ["a", "b", "_"], children: 1 });
				Exception.assert(!result.hasValue());
			}
		});

		it("count", async () => {
			const data = new Data();

			await data.insert("hello", [[["a", "b"], 1]]);
			await data.insert("hello", [[["a", "b"], 2]]);

			// get w/no options.
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"] });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), 2);
			}

			// count of 1 should return an array
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2]);
			}

			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 2 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// count greater than data
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 3 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// wrong key
			{
				const result = await data.get({ uid: "hello", key: ["a", "c"], count: 2 });
				Exception.assert(result.isEmpty());
			}
		});

		it("timestamp", async () => {
			const data = new Data();

			await data.insert("hello", [[["a", "b"], 1]], 1);
			await data.insert("hello", [[["a", "b"], 10]], 10);
			await data.insert("hello", [[["a", "b"], 2]], 2);
			await data.insert("hello", [[["a", "b"], 0]], 0);

			// read all
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [10, 2, 1, 0]);
			}

			// read all after 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, after: 2 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [10]);
			}

			// read 2 entries after 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 2, after: 0 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// read all entries after 10
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, after: 10 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
			}

			// read all before 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, before: 2 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0]);
			}

			// read 2 entries before 10
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 2, before: 10 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// read all entries before 0
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, before: 0 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
			}
		});

		it("include", async () => {
			const data = new Data();

			await data.insert("hello", [[["a", "b"], 1]]);
			await data.insert("hello", [[["a", "b", "c"], 10]]);
			await data.insert("hello", [[["a"], 2]]);
			await data.insert("hello", [[["a", "b", "d"], 0]]);

			// absolute
			{
				const result = await data.get({ uid: "hello", key: [], include: [["a", "b"]] });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [[["a", "b"], 1]]);
			}

			// multi values
			{
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					include: [
						["b", "c"],
						["b", "d"],
					],
				});
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["b", "c"], 10],
					[["b", "d"], 0],
				]);
			}

			// include empty path
			{
				const result = await data.get({ uid: "hello", key: ["a"], include: [["b", "c"], []] });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["b", "c"], 10],
					[[], 2],
				]);
			}

			// non existing path
			{
				const result = await data.get({ uid: "hello", key: ["a"], include: [["abc"], ["hello"]] });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
			}

			// absolute children
			{
				const result = await data.get({ uid: "hello", key: [], include: [["a", "b"]], children: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["a", "b"], 1],
					[["a", "b", "c"], 10],
					[["a", "b", "d"], 0],
				]);
			}

			// children
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], include: [["c"], ["d"]], children: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["c"], 10],
					[["d"], 0],
				]);
			}

			// include non existent
			{
				const result = await data.get({ uid: "hello", key: [], include: [["a", "b"], ["d"]], children: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["a", "b"], 1],
					[["a", "b", "c"], 10],
					[["a", "b", "d"], 0],
				]);
			}
		});

		it("external after", async () => {
			let useExternal = false;
			let externalData = [];
			const data = new Data({
				external: (..._) => {
					useExternal = true;
					return externalData;
				},
			});

			await data.insert("hello", [[["a"], 1]], 1);
			await data.insert("hello", [[["a"], 2]], 2);
			await data.insert("hello", [[["a"], 3]], 3);
			await data.insert("hello", [[["a"], 4]], 4);
			await data.insert("hello", [[["a"], 5]], 5);

			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], after: 1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4, 3, 2]);
				Exception.assert(!useExternal);
			}

			{
				useExternal = false;
				externalData = [];
				const result = await data.get({ uid: "hello", key: ["a"], after: 0, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4, 3, 2, 1]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [[0, 0]];
				const result = await data.get({ uid: "hello", key: ["a"], after: -1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3, 2, 1, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[0.5, 0.5],
					[0, 0],
				];
				const result = await data.get({ uid: "hello", key: ["a"], after: -1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [3, 2, 1, 0.5, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "hello", key: ["b"], after: 2, count: 5 });
				Exception.assert(!result.hasValue());
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "world", key: ["a"], after: 2, count: 5 });
				Exception.assert(!result.hasValue());
				Exception.assert(useExternal);
			}
		});

		it("external before", async () => {
			let useExternal = false;
			let externalData = [];
			const data = new Data({
				external: (uid, internal, count, after, before) => {
					useExternal = true;
					return externalData === null ? null : externalData.slice(0, count);
				},
			});

			await data.insert("hello", [[["a"], 1]], 1);
			await data.insert("hello", [[["a"], 2]], 2);
			await data.insert("hello", [[["a"], 3]], 3);
			await data.insert("hello", [[["a"], 4]], 4);
			await data.insert("hello", [[["a"], 5]], 5);

			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], before: 10, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4, 3, 2, 1]);
				Exception.assert(!useExternal);
			}

			{
				useExternal = false;
				externalData = [];
				const result = await data.get({ uid: "hello", key: ["a"], before: 5, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3, 2, 1]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [[0, 0]];
				const result = await data.get({ uid: "hello", key: ["a"], before: 5, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3, 2, 1, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[0.5, 0.5],
					[0, 0],
					[-1, -1],
					[-2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: 2, count: 3 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0.5, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[-2, -2],
					[-3, -3],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: -1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [-2, -3]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "hello", key: ["b"], before: 2, count: 5 });
				Exception.assert(!result.hasValue());
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "world", key: ["a"], before: 2, count: 5 });
				Exception.assert(!result.hasValue());
				Exception.assert(useExternal);
			}
		});

		it("external before/after", async () => {
			let useExternal = false;
			let externalData = [];
			const data = new Data({
				external: (uid, internal, count, after, before) => {
					useExternal = true;
					return externalData === null ? null : externalData.slice(0, count);
				},
			});

			await data.insert("hello", [[["a"], 1]], 1);
			await data.insert("hello", [[["a"], 2]], 2);
			await data.insert("hello", [[["a"], 3]], 3);
			await data.insert("hello", [[["a"], 4]], 4);
			await data.insert("hello", [[["a"], 5]], 5);

			// ask :  [ ]
			// data: [ local ][ extern ]
			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], before: 5, after: 2, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3]);
				Exception.assert(!useExternal);
			}

			// ask : [ ]
			// data:    [ local ][ extern ]
			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], before: 10, after: 5, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
				Exception.assert(!useExternal);
			}

			// ask : [     ]
			// data:   [ local ][ extern ]
			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], before: 9, after: 3, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4]);
				Exception.assert(!useExternal);
			}

			// ask :            [  ]
			// data: [ local ][ extern ]
			{
				useExternal = false;
				externalData = [
					[-1, -1],
					[-2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: 0, after: -3, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [-1, -2]);
				Exception.assert(useExternal);
			}

			// ask :       [     ]
			// data: [ local ][ extern ]
			{
				useExternal = false;
				externalData = [
					[0, 0],
					[-1, -1],
					[-2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: 2, after: -3, count: 4 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0, -1, -2]);
				Exception.assert(useExternal);
			}

			// ask :       [     ]
			// data: [ local ][ extern ]
			// using downsampling on the local data
			{
				useExternal = false;
				externalData = [
					[0, 0],
					[-2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: 4, after: -3, count: 4 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [3, 1, 0, -2]);
				Exception.assert(useExternal);
			}

			// ask :       [     ]
			// data: [ local ][ extern ]
			// using upsampling on the local data
			{
				useExternal = false;
				externalData = [
					[0, 0],
					[-2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: 4, after: -3, count: 10 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [3, 2, 1, 0, -2]);
				Exception.assert(useExternal);
			}

			// ask :       [     ]
			// data: [ local ][ extern ]
			// single local data.
			{
				useExternal = false;
				externalData = [
					[0, 0],
					[-2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: 2, after: -3, count: 2 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0]);
				Exception.assert(useExternal);
			}
		});
	});
});
