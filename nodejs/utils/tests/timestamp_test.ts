import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { timestampMs, timestampS, timestampUs } from "#bzd/nodejs/utils/timestamp.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "timestamp");

describe("timestamp", () => {
    describe("timestampMs", () => {
        it("Returns the current time in milliseconds", () => {
            const before = Date.now();
            const timestamp = timestampMs();
            const after = Date.now();
            Exception.assert(timestamp >= before && timestamp <= after, "Timestamp out of range");
        });
    });
    describe("timestampS", () => {
        it("Returns the current time in seconds", () => {
            const before = Date.now() / 1000;
            const timestamp = timestampS();
            const after = Date.now() / 1000;
            Exception.assert(timestamp >= before && timestamp <= after, "Timestamp out of range");
        });
    });
    describe("timestampUs", () => {
        it("Returns the current time in microseconds", () => {
            // The microsecond value may be slightly below the truncated millisecond
            // boundary, so allow a tolerance of one millisecond around it.
            const diff = Math.abs(timestampUs() - timestampMs() * 1000);
            Exception.assert(diff <= 1000, "Timestamp out of range: diff {}", diff);
        });

        it("Has a sub-millisecond precision", () => {
            // The low digits must vary, otherwise this is just a scaled millisecond value.
            const samples = new Set(Array.from({ length: 100 }, () => timestampUs() % 1000));
            Exception.assert(samples.size > 1, "Timestamp must carry microsecond precision");
        });
    });
});