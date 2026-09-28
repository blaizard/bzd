import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import SignalParser from "#bzd/nodejs/db/data/notifications/parser.js";

const Exception = ExceptionFactory("test", "db", "data", "notifications", "parser");

/// Build an evaluation context from a map of internal keys to values.
const makeCtx = (values, overrides = {}) =>
	Object.assign(
		{
			variant: null,
			timestamp: 0,
			memory: { lastResult: false, durationSince: null },
			getValue: (internalKey) => values[internalKey],
		},
		overrides,
	);

/// Build an internal key from segments.
const key = (...segments) => segments.join("\x01");

describe("SignalParser", () => {
	describe("parse", () => {
		it("compares a max aggregation to a number", () => {
			const signal = SignalParser.parse("max(/data/temperature/{name}) > 50");
			Exception.assertEqual(signal.variable, "name");
			Exception.assertEqual(signal.dependencies, [["data", "temperature", "{name}"]]);

			const values = { [key("data", "temperature", "main")]: [40, 55] };
			Exception.assertEqual(signal.evaluate(makeCtx(values, { variant: "main" })), true);
			Exception.assertEqual(
				signal.evaluate(makeCtx({ [key("data", "temperature", "main")]: [30, 40] }, { variant: "main" })),
				false,
			);
		});

		it("parses a duration signal and tracks the elapsed time", () => {
			const signal = SignalParser.parse("duration(max(/data/cpu/{name}) > 0.95, 3600)");
			Exception.assertEqual(signal.variable, "name");
			Exception.assertEqual(signal.dependencies, [["data", "cpu", "{name}"]]);

			const values = { [key("data", "cpu", "main")]: [0.99] };
			const memory = { lastResult: false, durationSince: null };

			// The condition becomes true, the duration is not reached yet.
			Exception.assertEqual(signal.evaluate(makeCtx(values, { variant: "main", timestamp: 0, memory: memory })), false);
			Exception.assertEqual(memory.durationSince, 0);

			// The condition has been true for exactly 1h.
			Exception.assertEqual(
				signal.evaluate(makeCtx(values, { variant: "main", timestamp: 3600 * 1000, memory: memory })),
				true,
			);

			// A low value resets the duration.
			const lowValues = { [key("data", "cpu", "main")]: [0.1] };
			Exception.assertEqual(
				signal.evaluate(makeCtx(lowValues, { variant: "main", timestamp: 3600 * 1000 + 1, memory: memory })),
				false,
			);
			Exception.assertEqual(memory.durationSince, null);
		});

		it("compares two paths", () => {
			const signal = SignalParser.parse("/data/memory/{name}/total > /data/memory/{name}/used");
			Exception.assertEqual(signal.variable, "name");
			Exception.assertEqual(signal.dependencies, [
				["data", "memory", "{name}", "total"],
				["data", "memory", "{name}", "used"],
			]);

			const values = { [key("data", "memory", "main", "total")]: 8e9, [key("data", "memory", "main", "used")]: 9e9 };
			Exception.assertEqual(signal.evaluate(makeCtx(values, { variant: "main" })), false);
			values[key("data", "memory", "main", "used")] = 7e9;
			Exception.assertEqual(signal.evaluate(makeCtx(values, { variant: "main" })), true);
		});

		it("parses a signal without variables", () => {
			const signal = SignalParser.parse("/data/hello > 0");
			Exception.assertEqual(signal.variable, null);
			Exception.assertEqual(signal.dependencies, [["data", "hello"]]);
			Exception.assertEqual(signal.evaluate(makeCtx({ [key("data", "hello")]: 5 })), true);
			Exception.assertEqual(signal.evaluate(makeCtx({ [key("data", "hello")]: -1 })), false);
		});

		it("supports all comparison operators", () => {
			for (const op of [">", "<", ">=", "<=", "==", "!="]) {
				const signal = SignalParser.parse("/data/a " + op + " /data/b");
				Exception.assertEqual(
					signal.evaluate(makeCtx({ [key("data", "a")]: 2, [key("data", "b")]: 1 })),
					op === ">" || op === ">=" || op === "!=",
				);
				Exception.assertEqual(
					signal.evaluate(makeCtx({ [key("data", "a")]: 1, [key("data", "b")]: 1 })),
					op === ">=" || op === "<=" || op === "==",
				);
			}
		});

		it("supports the avg, min, sum and latest functions", () => {
			const cases = [
				["avg", [1, 2, 3], 2],
				["min", [1, 2, 3], 1],
				["sum", [1, 2, 3], 6],
			];
			for (const [fn, array, result] of cases) {
				const signal = SignalParser.parse(fn + "(/data/x/{name}) == " + result);
				Exception.assertEqual(signal.dependencies, [["data", "x", "{name}"]]);
				Exception.assertEqual(
					signal.evaluate(makeCtx({ [key("data", "x", "main")]: array }, { variant: "main" })),
					true,
				);
			}
			const latest = SignalParser.parse("latest(/data/x) == 3");
			Exception.assertEqual(latest.evaluate(makeCtx({ [key("data", "x")]: 3 })), true);
			Exception.assertEqual(latest.evaluate(makeCtx({ [key("data", "x")]: 1 })), false);
		});

		it("returns false when a value is missing", () => {
			const signal = SignalParser.parse("/data/a > /data/b");
			Exception.assertEqual(signal.evaluate(makeCtx({ [key("data", "a")]: 5 })), false);
		});

		it("deduplicates dependencies", () => {
			const signal = SignalParser.parse("/data/x > /data/x");
			Exception.assertEqual(signal.dependencies, [["data", "x"]]);
		});
	});

	describe("errors", () => {
		const invalidSignals = {
			"unknown function": "unknown(/data/x) > 1",
			"empty signal": "",
			"trailing content": "max(/data/x) > 1 hello",
			"missing closing parenthesis": "max(/data/x",
			"multiple variables": "/data/{a}/x > /data/{b}/y",
			"malformed variable": "/data/{name",
			"empty path": "/",
			"empty segment": "/data//x > 0",
			"invalid segment": "/data/.. > 0",
			"unexpected character": "max(/data/x) > !",
			"wrong arity max": "max() > 1",
			"wrong arity max two args": "max(/data/x, /data/y) > 1",
			"wrong arity duration": "duration(/data/x > 1)",
			"duration first arg not a comparison": "duration(/data/x, 60)",
			"duration second arg not a number": "duration(/data/x > 1, /data/y)",
			"aggregator arg not a path": "max(5) > 1",
		};
		for (const [description, signal] of Object.entries(invalidSignals)) {
			it(description, () => {
				Exception.assertThrows(() => {
					SignalParser.parse(signal);
				});
			});
		}
	});
});
