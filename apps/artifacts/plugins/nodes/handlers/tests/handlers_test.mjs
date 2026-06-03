import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Handlers from "#bzd/apps/artifacts/plugins/nodes/handlers/handlers.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "handlers");

describe("Handlers", () => {
	it("no handlers", () => {
		const handlers = new Handlers({});
		const result = handlers.process([
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

	it("history", () => {
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
		const result = handlers.process([
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

	it("toString", () => {
		const handlers = new Handlers({
			"/a": {
				toString: "{b} != {d}",
			},
		});
		const result = handlers.process([
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

	it("toString failed", () => {
		const handlers = new Handlers({
			"/a": {
				toString: "{b} != {d}",
			},
		});
		Exception.assertThrows(() => {
			handlers.process([[["a", "b"], "value1"]]);
		});
	});

	it("combined", () => {
		const handlers = new Handlers({
			"/a": {
				toString: "{b} != {d}",
				history: 2,
			},
			"/c": {
				history: 7,
			},
		});
		const result = handlers.process([
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

	it("validation root", () => {
		const handlers = new Handlers({
			"/a": {
				validation: {
					type: "string",
				},
			},
		});
		handlers.process([[["a"], "I am a string"]]);
		Exception.assertThrows(() => {
			handlers.process([[["a"], 12]]);
		});
	});

	it("validation invalid json", () => {
		const handlers = new Handlers({
			"/a": {
				validation: {},
			},
		});
		Exception.assertThrowsWithMatch(() => {
			handlers.process([
				[["a", "b"], "value1"],
				[["a"], "value2"],
			]);
		}, "not json-compatible");
	});

	it("validation complex", () => {
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
		handlers.process([
			[
				["data", "cpu", "main"],
				[0, 1, 0.5],
			],
		]);
		Exception.assertThrowsWithMatch(() => {
			handlers.process([
				[
					["data", "cpu", "main"],
					[0, 1.1, 0.5],
				],
			]);
		}, "not 1.1");
	});
});
