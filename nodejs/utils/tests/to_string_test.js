import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { UnitToStringFactory, UCUMToString } from "#bzd/nodejs/utils/to_string.js";

const Exception = ExceptionFactory("test", "to_string");

describe("ToString", () => {
	describe("UnitToStringFactory", () => {
		it("Simple", () => {
			Exception.assertEqual(
				UnitToStringFactory.makeFromPreset(2000, "k", "Hz", UnitToStringFactory.presetUCUMMetrics),
				"2.0MHz",
			);
			Exception.assertEqual(
				UnitToStringFactory.makeFromPreset(0.0000001, "k", "g", UnitToStringFactory.presetUCUMMetrics),
				"100.0μg",
			);
		});
	});

	describe("UCUMToString", () => {
		it("Simple", () => {
			Exception.assertEqual(UCUMToString(2000, "kHz"), "2.0MHz");
			Exception.assertEqual(UCUMToString(2000, "Hz"), "2.0kHz");
			Exception.assertEqual(UCUMToString(0.1, "min"), "6.0s");
			Exception.assertEqual(UCUMToString(2000, "ns"), "2.0μs");
			Exception.assertEqual(UCUMToString(1800, "s"), "30.0min");
			Exception.assertEqual(UCUMToString(2048, "KiBy"), "2.0MiB");
			Exception.assertEqual(UCUMToString(0.25, "%"), "25.0%");
			Exception.assertEqual(UCUMToString(20, "Cel"), "20.0°C");
		});
	});
});
