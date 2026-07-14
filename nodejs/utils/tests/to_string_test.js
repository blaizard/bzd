import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { UnitToString } from "#bzd/nodejs/utils/to_string.js";

const Exception = ExceptionFactory("test", "to_string");

describe("ToString", () => {
	describe("UnitToString", () => {
		it("Simple", () => {
			Exception.assertEqual(UnitToString.factory(2000, "k", "Hz", UnitToString.presetMetrics), "2.0MHz");
			Exception.assertEqual(UnitToString.factory(0.0000001, "k", "g", UnitToString.presetMetrics), "100.0μg");
		});
	});
});
