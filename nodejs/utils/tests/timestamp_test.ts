import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { timestampMs, timestampS } from "#bzd/nodejs/utils/timestamp.js";

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
});