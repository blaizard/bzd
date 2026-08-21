import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Result from "#bzd/nodejs/utils/result.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "result");

describe("Result", () => {
    describe("hasValue/hasError", () => {
        it("A default result holds a value", () => {
            const result = new Result();
            Exception.assertEqual(result.hasValue(), true);
            Exception.assertEqual(result.hasError(), false);
        });
        it("A result with a value", () => {
            const result = new Result(42);
            Exception.assertEqual(result.hasValue(), true);
            Exception.assertEqual(result.hasError(), false);
        });
        it("A result with an error", () => {
            const result = Result.makeError("some error");
            Exception.assertEqual(result.hasValue(), false);
            Exception.assertEqual(result.hasError(), true);
        });
    });
    describe("value", () => {
        it("Returns the stored value", () => {
            const result = new Result({ a: 1 });
            Exception.assertEqual(result.value(), { a: 1 });
        });
        it("Returns the default value", () => {
            Exception.assertEqual(new Result().value(), true);
        });
        it("Throws on a result containing an error", () => {
            Exception.assertThrows(() => Result.makeError("some error").value());
        });
    });
    describe("error", () => {
        it("Returns the stored error", () => {
            const result = Result.makeError("some error");
            Exception.assertEqual(result.error(), "some error");
        });
        it("Throws on a result containing a value", () => {
            Exception.assertThrows(() => new Result(42).error());
        });
    });
    describe("makeErrorString", () => {
        it("Formats the error message", () => {
            const result = Result.makeErrorString("Error {} with {}.", 1, "foo");
            Exception.assertEqual(result.hasError(), true);
            Exception.assertEqual(result.error(), "Error 1 with foo.");
        });
    });
    describe("toString", () => {
        it("Result with a value", () => {
            Exception.assertEqual(new Result(42).toString(), "<Result value=42>");
        });
        it("Result with an error", () => {
            Exception.assertEqual(Result.makeError("foo").toString(), "<Result error=foo>");
        });
    });
});