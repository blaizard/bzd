import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Optional from "#bzd/nodejs/utils/optional.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "optional");

describe("Optional", () => {
    describe("isEmpty/hasValue", () => {
        it("Default constructor creates an empty optional", () => {
            const optional = new Optional();
            Exception.assertEqual(optional.isEmpty(), true);
            Exception.assertEqual(optional.hasValue(), false);
        });
        it("A value makes the optional non-empty", () => {
            const optional = new Optional(42);
            Exception.assertEqual(optional.isEmpty(), false);
            Exception.assertEqual(optional.hasValue(), true);
        });
        it("A falsy value still counts as a value", () => {
            const optional = new Optional(0);
            Exception.assertEqual(optional.hasValue(), true);
        });
    });
    describe("value", () => {
        it("Returns the stored value", () => {
            const optional = new Optional({ a: 1 });
            Exception.assertEqual(optional.value(), { a: 1 });
        });
        it("Throws on an empty optional", () => {
            Exception.assertThrows(() => new Optional().value(), "Cannot access the value of an empty optional");
        });
    });
    describe("toString", () => {
        it("Empty optional", () => {
            Exception.assertEqual(new Optional().toString(), "<Optional>");
        });
        it("Optional with a value", () => {
            Exception.assertEqual(new Optional(42).toString(), "<Optional value=42>");
        });
    });
});