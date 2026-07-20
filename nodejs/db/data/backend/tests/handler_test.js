import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import MockServerContext from "#bzd/nodejs/core/http/mock/server_context.js";
import Data from "#bzd/nodejs/db/data/data.js";
import { handleDataGet, getDataGetInputsFromQuery } from "#bzd/nodejs/db/data/backend/handler.js";

const Exception = ExceptionFactory("test", "db", "data", "backend", "handler");

describe("handleDataGet", () => {
	it("throws when uid is undefined", async () => {
		const data = new Data();
		await Exception.assertThrows(async () => {
			await handleDataGet(data, { key: ["a"] });
		});
	});

	it("returns {data: value()} for a non-empty get", async () => {
		const data = new Data();
		data.insert("u", [[["a"], 42]]);
		const out = await handleDataGet(data, { uid: "u", key: ["a"] });
		Exception.assertEqual(out, { data: 42 });
	});

	it("returns null when get is empty", async () => {
		const data = new Data();
		const out = await handleDataGet(data, { uid: "u", key: ["nonexistent"] });
		Exception.assertEqual(out, null);
	});

	it("adds output.timestamp when metadata=true", async () => {
		const data = new Data();
		data.insert("u", [[["a"], 42]]);
		const out = await handleDataGet(data, { uid: "u", key: ["a"], metadata: true });
		Exception.assert(typeof out.timestamp === "number");
		Exception.assert(Array.isArray(out.data));
		Exception.assertEqual(out.data[1], 42);
	});

	it("normalizes missing key to []", async () => {
		const data = new Data();
		data.insert("u", [[[], "root"]]);
		const out = await handleDataGet(data, { uid: "u" });
		Exception.assertEqual(out.data, "root");
	});

	it("passes key through to get()", async () => {
		const data = new Data();
		data.insert("u", [[["a", "b", "c"], 99]]);
		const out = await handleDataGet(data, { uid: "u", key: ["a", "b", "c"] });
		Exception.assertEqual(out.data, 99);
	});

	it("applies documented defaults", async () => {
		const data = new Data();
		data.insert("u", [[["a"], 1]]);
		const out = await handleDataGet(data, { uid: "u", key: ["a"] });
		Exception.assertEqual(out.data, 1);
		Exception.assert(!("timestamp" in out));
	});

	it("forwards children verbatim", async () => {
		const data = new Data();
		data.insert("u", [
			[["a", "b"], 1],
			[["a", "c"], 2],
		]);
		const out = await handleDataGet(data, { uid: "u", key: ["a"], children: 1 });
		Exception.assert(Array.isArray(out.data));
		Exception.assertEqual(out.data.length, 2);
	});

	it("forwards count verbatim", async () => {
		const data = new Data();
		data.insert("u", [[["a"], 1]]);
		data.insert("u", [[["a"], 2]]);
		const out = await handleDataGet(data, { uid: "u", key: ["a"], count: 2 });
		Exception.assert(Array.isArray(out.data));
		Exception.assertEqual(out.data.length, 2);
	});

	it("calls getChildren (not get) when keys=true", async () => {
		const data = new Data();
		data.insert("u", [
			[["a"], 1],
			[["b"], 2],
		]);
		const out = await handleDataGet(data, { uid: "u", children: 0, keys: true });
		Exception.assert(Array.isArray(out.data));
	});

	it("returns null when getChildren returns null in keys mode", async () => {
		const data = new Data();
		data.insert("u", [[["a"], 42]]);
		const out = await handleDataGet(data, { uid: "u", key: ["nonexistent"], keys: true });
		Exception.assertEqual(out, null);
	});

	const conflicts = [
		["metadata", true],
		["count", 1],
		["after", 1],
		["before", 1],
		["include", [[]]],
		["sampling", "linear"],
	];
	for (const [field, value] of conflicts) {
		it(`rejects keys=true with ${field} set`, async () => {
			const data = new Data();
			const args = { uid: "u", keys: true, [field]: value };
			await Exception.assertThrows(async () => {
				await handleDataGet(data, args);
			});
		});
	}

	it("returns getChildren format when keys=true on valid key", async () => {
		const data = new Data();
		data.insert("u", [
			[["a"], 1],
			[["b"], 2],
		]);
		const out = await handleDataGet(data, { uid: "u", children: 0, keys: true });
		Exception.assert(Array.isArray(out.data));
		Exception.assertEqual(out.data.length, 2);
		for (const entry of out.data) {
			Exception.assert("key" in entry);
			Exception.assert("leaf" in entry);
		}
	});

	it("defaults keys to false when omitted", async () => {
		const data = new Data();
		data.insert("u", [[["a"], 42]]);
		const out = await handleDataGet(data, { uid: "u", key: ["a"] });
		Exception.assertEqual(out, { data: 42 });
	});
});

describe("getDataGetInputsFromQuery", () => {
	it("maps keys, metadata, children, count, sampling through their transforms", () => {
		const ctx = MockServerContext.make({
			query: { metadata: "1", children: "3", count: "2", sampling: "raw", keys: "1" },
		});
		const out = getDataGetInputsFromQuery(ctx);
		Exception.assertEqual(out.metadata, true);
		Exception.assertEqual(out.children, 3);
		Exception.assertEqual(out.count, 2);
		Exception.assertEqual(out.sampling, "raw");
		Exception.assertEqual(out.keys, true);
	});

	it("parses include into an array of decoded key arrays", () => {
		const ctx = MockServerContext.make({ query: { include: "/a/b,/a/d/e" } });
		const out = getDataGetInputsFromQuery(ctx);
		const expected = [
			["a", "b"],
			["a", "d", "e"],
		];
		Exception.assertEqual(JSON.stringify(out.include), JSON.stringify(expected));
	});
});
