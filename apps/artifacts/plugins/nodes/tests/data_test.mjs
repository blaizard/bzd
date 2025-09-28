import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";
import Utils from "#bzd/apps/artifacts/common/utils.mjs";

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
			// Insert expired data.
			await data.insert("hello", [[["a", "d", "expired"], 42]], 10);

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
				Exception.assertEqual(result.value()[2], 1); // not expired
			}

			// get w/metadata expired
			{
				const result = await data.get({ uid: "hello", key: ["a", "d", "expired"], metadata: true });
				Exception.assert(result.hasValue());
				Exception.assert(typeof result.value()[0] == "number");
				Exception.assertEqual(result.value()[1], 42);
				Exception.assertEqual(result.value()[2], 0); // expired
			}

			// get w/children
			{
				const result = await data.get({ uid: "hello", key: ["a", "c"], children: 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["d"], 13],
					[["e"], 1],
				]);
			}

			// get w/children with level 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "c"], children: 2 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[["d"], 13],
					[["d", "f"], 4],
					[["e"], 1],
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
			const timestamp = Utils.timestampMs();

			await data.insert("hello", [[["a", "b"], 1]], timestamp - 2);
			await data.insert("hello", [[["a", "b"], 10]], timestamp);
			await data.insert("hello", [[["a", "b"], 2]], timestamp - 1);
			await data.insert("hello", [[["a", "b"], 0]], timestamp - 3);

			// read all
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [10, 2, 1, 0]);
			}

			// read all after 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, after: timestamp - 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [10]);
			}

			// read 2 entries after 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 2, after: timestamp - 3 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// read all entries after 10
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, after: timestamp });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
			}

			// read all before 2
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, before: timestamp - 1 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0]);
			}

			// read 2 entries before 10
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 2, before: timestamp });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [2, 1]);
			}

			// read all entries before 0
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, before: timestamp - 3 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
			}
		});

		it("expired", async () => {
			const data = new Data();
			const timestamp = Utils.timestampMs();
			const expiredTimestamp = timestamp - 1000 * 1000;

			await data.insert("hello", [[["a", "b"], 1]], timestamp);
			await data.insert("hello", [[["a", "b"], 10]], expiredTimestamp);

			// read all w/o metadata
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1]);
			}

			// read all w/metadata
			{
				const result = await data.get({ uid: "hello", key: ["a", "b"], count: 10, metadata: true });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [
					[timestamp, 1, 1],
					[expiredTimestamp, 10, 0],
				]);
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
			const timestamp = Utils.timestampMs();

			await data.insert("hello", [[["a"], 1]], timestamp + 1);
			await data.insert("hello", [[["a"], 2]], timestamp + 2);
			await data.insert("hello", [[["a"], 3]], timestamp + 3);
			await data.insert("hello", [[["a"], 4]], timestamp + 4);
			await data.insert("hello", [[["a"], 5]], timestamp + 5);

			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], after: timestamp + 1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4, 3, 2]);
				Exception.assert(!useExternal);
			}

			{
				useExternal = false;
				externalData = [];
				const result = await data.get({ uid: "hello", key: ["a"], after: timestamp, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4, 3, 2, 1]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [[timestamp, 0]];
				const result = await data.get({ uid: "hello", key: ["a"], after: timestamp - 1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3, 2, 1, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[timestamp + 0.5, 0.5],
					[timestamp, 0],
				];
				const result = await data.get({ uid: "hello", key: ["a"], after: timestamp - 1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [3, 2, 1, 0.5, 0]);
				Exception.assert(useExternal);
			}

			{
				// overlapping data from external.

				useExternal = false;
				externalData = [
					[timestamp + 1, 1],
					[timestamp + 0.5, 0.5],
					[timestamp, 0],
				];
				const result = await data.get({ uid: "hello", key: ["a"], after: timestamp - 1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [3, 2, 1, 0.5, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "hello", key: ["b"], after: timestamp + 2, count: 5 });
				Exception.assert(!result.hasValue());
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "world", key: ["a"], after: timestamp + 2, count: 5 });
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
			const timestamp = Utils.timestampMs();

			await data.insert("hello", [[["a"], 1]], timestamp + 1);
			await data.insert("hello", [[["a"], 2]], timestamp + 2);
			await data.insert("hello", [[["a"], 3]], timestamp + 3);
			await data.insert("hello", [[["a"], 4]], timestamp + 4);
			await data.insert("hello", [[["a"], 5]], timestamp + 5);

			{
				useExternal = false;
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp + 10, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4, 3, 2, 1]);
				Exception.assert(!useExternal);
			}

			{
				useExternal = false;
				externalData = [];
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp + 5, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3, 2, 1]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [[timestamp, 0]];
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp + 5, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3, 2, 1, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[timestamp + 0.5, 0.5],
					[timestamp, 0],
					[timestamp - 1, -1],
					[timestamp - 2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp + 2, count: 3 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0.5, 0]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[timestamp - 2, -2],
					[timestamp - 3, -3],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp - 1, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [-2, -3]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = [
					[timestamp, 0],
					[timestamp - 1, -1],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp + 2, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0, -1]);
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "hello", key: ["b"], before: timestamp + 2, count: 5 });
				Exception.assert(!result.hasValue());
				Exception.assert(useExternal);
			}

			{
				useExternal = false;
				externalData = null;
				const result = await data.get({ uid: "world", key: ["a"], before: timestamp + 2, count: 5 });
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
			const timestamp = Utils.timestampMs();

			await data.insert("hello", [[["a"], 1]], timestamp + 1);
			await data.insert("hello", [[["a"], 2]], timestamp + 2);
			await data.insert("hello", [[["a"], 3]], timestamp + 3);
			await data.insert("hello", [[["a"], 4]], timestamp + 4);
			await data.insert("hello", [[["a"], 5]], timestamp + 5);

			// ask :  [ ]
			// data: [ local ][ extern ]
			{
				useExternal = false;
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 5,
					after: timestamp + 2,
					count: 5,
				});
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [4, 3]);
				Exception.assert(!useExternal);
			}

			// ask : [ ]
			// data:    [ local ][ extern ]
			{
				useExternal = false;
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 10,
					after: timestamp + 5,
					count: 5,
				});
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), []);
				Exception.assert(!useExternal);
			}

			// ask : [     ]
			// data:   [ local ][ extern ]
			{
				useExternal = false;
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 9,
					after: timestamp + 3,
					count: 5,
				});
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [5, 4]);
				Exception.assert(!useExternal);
			}

			// ask :            [  ]
			// data: [ local ][ extern ]
			{
				useExternal = false;
				externalData = [
					[timestamp - 1, -1],
					[timestamp - 2, -2],
				];
				const result = await data.get({ uid: "hello", key: ["a"], before: timestamp, after: timestamp - 3, count: 5 });
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [-1, -2]);
				Exception.assert(useExternal);
			}

			// ask :       [     ]
			// data: [ local ][ extern ]
			{
				useExternal = false;
				externalData = [
					[timestamp, 0],
					[timestamp - 1, -1],
					[timestamp - 2, -2],
				];
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 2,
					after: timestamp - 3,
					count: 4,
				});
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
					[timestamp, 0],
					[timestamp - 2, -2],
				];
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 4,
					after: timestamp - 3,
					count: 4,
				});
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
					[timestamp, 0],
					[timestamp - 2, -2],
				];
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 4,
					after: timestamp - 3,
					count: 10,
				});
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
					[timestamp, 0],
					[timestamp - 2, -2],
				];
				const result = await data.get({
					uid: "hello",
					key: ["a"],
					before: timestamp + 2,
					after: timestamp - 3,
					count: 2,
				});
				Exception.assert(result.hasValue());
				Exception.assertEqual(result.value(), [1, 0]);
				Exception.assert(useExternal);
			}
		});

		it("getChildren", async () => {
			const data = new Data();

			await data.insert("hello", [[["a"], 1]]);
			await data.insert("hello", [[["b"], 2]]);
			await data.insert("hello", [[["c"], 3]]);
			await data.insert("hello", [[["c", "d"], 4]]);
			await data.insert("hello", [[["a", "b", "c"], 5]]);
			await data.insert("hello", [[["a", "c"], 6]]);

			{
				const result = await data.getChildren({ uid: "hello", key: [], children: 0, includeInner: false });
				Exception.assertEqual(result, []);
			}

			{
				const result = await data.getChildren({ uid: "hello", key: [], children: 0, includeInner: true });
				Exception.assertEqual(result, [
					{ key: ["a"], leaf: true },
					{ key: ["b"], leaf: true },
					{ key: ["c"], leaf: true },
				]);
			}

			{
				const result = await data.getChildren({ uid: "hello", key: [], children: 1, includeInner: false });
				Exception.assertEqual(result, [
					{ key: ["a"], leaf: true },
					{ key: ["b"], leaf: true },
					{ key: ["c"], leaf: true },
				]);
			}

			{
				const result = await data.getChildren({ uid: "hello", key: [], children: 2, includeInner: false });
				Exception.assertEqual(result, [
					{ key: ["a"], leaf: true },
					{ key: ["a", "c"], leaf: true },
					{ key: ["b"], leaf: true },
					{ key: ["c"], leaf: true },
					{ key: ["c", "d"], leaf: true },
				]);
			}

			{
				const result = await data.getChildren({ uid: "hello", key: [], children: 3, includeInner: false });
				Exception.assertEqual(result, [
					{ key: ["a"], leaf: true },
					{ key: ["a", "b", "c"], leaf: true },
					{ key: ["a", "c"], leaf: true },
					{ key: ["b"], leaf: true },
					{ key: ["c"], leaf: true },
					{ key: ["c", "d"], leaf: true },
				]);
			}

			{
				const result = await data.getChildren({ uid: "hello", key: ["a"], children: 2, includeInner: false });
				Exception.assertEqual(result, [
					{ key: [], leaf: true },
					{ key: ["b", "c"], leaf: true },
					{ key: ["c"], leaf: true },
				]);
			}

			{
				const result = await data.getChildren({ uid: "hello", key: ["hello"], children: 0, includeInner: false });
				Exception.assertEqual(result, null);
			}
		});
	});
});
