import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Handlers from "#bzd/nodejs/db/data/handlers/handlers.js";

const Exception = ExceptionFactory("test", "db", "data", "handlers");

describe("Handlers", () => {
	it("no handlers", async () => {
		const handlers = new Handlers({});
		const result = await handlers.process([
			[["a", "b"], "value1"],
			[["a"], "value2"],
			[["a", "c", "d"], "value3"],
			[["c"], "value4"],
			[["b", "l"], "value5"],
			[["a", "d"], "value6"],
		]);
		Exception.assertEqual(result, [
			[["a"], "value2", {}],
			[["a", "b"], "value1", {}],
			[["a", "c", "d"], "value3", {}],
			[["a", "d"], "value6", {}],
			[["b", "l"], "value5", {}],
			[["c"], "value4", {}],
		]);
	});

	it("history", async () => {
		const handlers = new Handlers({
			"/a": {
				history: 2,
			},
			"/a/c": {
				history: 3,
			},
			"/c": {
				history: 7,
			},
		});
		const result = await handlers.process([
			[["a", "b"], "value1"],
			[["a"], "value2"],
			[["a", "c", "d"], "value3"],
			[["c"], "value4"],
			[["b", "l"], "value5"],
			[["a", "d"], "value6"],
		]);
		Exception.assertEqual(result, [
			[["a"], "value2", { history: 2 }],
			[["a", "b"], "value1", { history: 2 }],
			[["a", "c", "d"], "value3", { history: 3 }],
			[["a", "d"], "value6", { history: 2 }],
			[["b", "l"], "value5", {}],
			[["c"], "value4", { history: 7 }],
		]);
	});

	it("history deep override fires after shallow block skip", async () => {
		const handlers = new Handlers({
			"/a": {
				history: 2,
			},
			"/a/c": {
				history: 3,
			},
		});
		const result = await handlers.process([
			[["a"], "valueA"],
			[["a", "c", "d"], "valueACD"],
			[["a", "d"], "valueAD"],
		]);
		Exception.assertEqual(result, [
			[["a"], "valueA", { history: 2 }],
			[["a", "c", "d"], "valueACD", { history: 3 }],
			[["a", "d"], "valueAD", { history: 2 }],
		]);
	});

	it("toString", async () => {
		const handlers = new Handlers({
			"/a": {
				toString: "{b} != {d}",
			},
		});
		const result = await handlers.process([
			[["a", "name", "b"], "value1"],
			[["c"], "value4"],
			[["b", "l"], "value5"],
			[["a", "name", "d"], "value6"],
		]);
		Exception.assertEqual(result, [
			[["a", "name"], "value1 != value6", {}],
			[["b", "l"], "value5", {}],
			[["c"], "value4", {}],
		]);
	});

	it("toString failed", async () => {
		const handlers = new Handlers({
			"/a": {
				toString: "{b} != {d}",
			},
		});
		await Exception.assertThrows(async () => {
			await handlers.process([[["a", "b"], "value1"]]);
		});
	});

	it("combined", async () => {
		const handlers = new Handlers({
			"/a": {
				toString: "{b} != {d}",
				history: 2,
			},
			"/c": {
				history: 7,
			},
		});
		const result = await handlers.process([
			[["a", "name", "b"], "value1"],
			[["c"], "value4"],
			[["b", "l"], "value5"],
			[["a", "name", "d"], "value6"],
		]);
		Exception.assertEqual(result, [
			[["a", "name"], "value1 != value6", { history: 2 }],
			[["b", "l"], "value5", {}],
			[["c"], "value4", { history: 7 }],
		]);
	});

	it("toString multi-group", async () => {
		const handlers = new Handlers({
			"/a": { toString: "{b} != {d}" },
		});
		const result = await handlers.process([
			[["a", "x", "b"], 1],
			[["a", "x", "d"], 2],
			[["a", "y", "b"], 3],
			[["a", "y", "d"], 4],
			[["b", "l"], 5],
			[["c"], 6],
		]);
		Exception.assertEqual(result, [
			[["a", "x"], "1 != 2", {}],
			[["a", "y"], "3 != 4", {}],
			[["b", "l"], 5, {}],
			[["c"], 6, {}],
		]);
	});

	it("group", async () => {
		const handlers = new Handlers({
			"/a": {
				group: true,
			},
		});
		const result = await handlers.process([
			[["a", "name", "b"], "value1"],
			[["c"], "value4"],
			[["a", "name", "d"], "value6"],
			[["a", "other", "b"], "value7"],
		]);
		Exception.assertEqual(result, [
			[["a", "name"], { b: "value1", d: "value6" }, {}],
			[["a", "other"], { b: "value7" }, {}],
			[["c"], "value4", {}],
		]);
	});

	it("group scalar leaves untouched", async () => {
		const handlers = new Handlers({
			"/a": {
				group: true,
			},
		});
		const result = await handlers.process([
			[["a", "name"], "value1"],
			[["a", "name", "b"], "value2"],
		]);
		Exception.assertEqual(result, [
			[["a", "name"], "value1", {}],
			[["a", "name"], { b: "value2" }, {}],
		]);
	});

	it("group combined", async () => {
		const handlers = new Handlers({
			"/a": {
				group: true,
				history: 2,
			},
		});
		const result = await handlers.process([
			[["a", "name", "b"], "value1"],
			[["c"], "value4"],
			[["a", "name", "d"], "value6"],
		]);
		Exception.assertEqual(result, [
			[["a", "name"], { b: "value1", d: "value6" }, { history: 2 }],
			[["c"], "value4", {}],
		]);
	});

	it("validation root", async () => {
		const handlers = new Handlers({
			"/a": {
				validation: {
					type: "string",
				},
			},
		});
		await handlers.process([[["a"], "I am a string"]]);
		await Exception.assertThrows(async () => {
			await handlers.process([[["a"], 12]]);
		});
	});

	it("validation invalid json", async () => {
		const handlers = new Handlers({
			"/a": {
				validation: {},
			},
		});
		await Exception.assertThrowsWithMatch(async () => {
			await handlers.process([
				[["a", "b"], "value1"],
				[["a"], "value2"],
			]);
		}, "not json-compatible");
	});

	it("validation complex", async () => {
		const handlers = new Handlers({
			"/data/cpu": {
				validation: {
					additionalProperties: {
						items: {
							maximum: 1,
							minimum: 0,
							type: "number",
						},
						type: "array",
					},
					description: "CPU utilization per core, values are in the range [0, 1].",
					type: "object",
				},
			},
		});
		await handlers.process([
			[
				["data", "cpu", "main"],
				[0, 1, 0.5],
			],
		]);
		await Exception.assertThrowsWithMatch(async () => {
			await handlers.process([
				[
					["data", "cpu", "main"],
					[0, 1.1, 0.5],
				],
			]);
		}, "not 1.1");
	});
});
