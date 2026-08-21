import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import {
    UnitToStringFactory,
    UCUMToString,
    frequencyToString,
    bytesToString,
    timeToString,
    dateToString,
    dateToDefaultString,
    capitalize,
} from "#bzd/nodejs/utils/to_string.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

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

        it("Composed", () => {
            Exception.assertEqual(UCUMToString(2000, "mg/s"), "2.0g/s");
        });

        it("Returns the default value for an unknown unit", () => {
            Exception.assertEqual(UCUMToString(10, "unknown", "-"), "-");
            Exception.assertEqual(UCUMToString(10, "unknown"), undefined);
        });
    });

    describe("frequencyToString", () => {
        it("Formats a frequency", () => {
            Exception.assertEqual(frequencyToString(2000), "2.0kHz");
        });
    });

    describe("bytesToString", () => {
        it("Formats a byte count", () => {
            Exception.assertEqual(bytesToString(2048), "2.0KiB");
            Exception.assertEqual(bytesToString(1024), "1024.0B");
        });
    });

    describe("timeToString", () => {
        it("Formats a duration in seconds", () => {
            Exception.assertEqual(timeToString(90), "1.5min");
            Exception.assertEqual(timeToString(0.5), "500.0ms");
        });
    });

    describe("dateToString", () => {
        it("Formats the given timestamp", () => {
            const timestamp = new Date(2024, 0, 2, 3, 4, 5, 6).getTime();
            Exception.assertEqual(dateToString("{y}-{m}-{d} {h}:{min}:{s}.{ms}", timestamp), "2024-1-2 3:4:5.6");
        });
    });

    describe("dateToDefaultString", () => {
        it("Formats the given timestamp in the default format", () => {
            const timestamp = new Date(2024, 0, 2, 3, 4, 5, 6).getTime();
            Exception.assertEqual(dateToDefaultString(timestamp), "2024-01-02 03:04:05.6");
        });
    });

    describe("capitalize", () => {
        it("Capitalizes the first letter", () => {
            Exception.assertEqual(capitalize("hello"), "Hello");
            Exception.assertEqual(capitalize("Hello"), "Hello");
            Exception.assertEqual(capitalize(""), "");
        });
    });
});