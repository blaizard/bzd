import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { regexprEscape } from "#bzd/nodejs/utils/regexpr.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "regexpr");

describe("regexprEscape", () => {
    it("Leaves plain strings unchanged", () => {
        Exception.assertEqual(regexprEscape("plain"), "plain");
    });
    it("Escapes regex metacharacters", () => {
        Exception.assertEqual(regexprEscape("a.b*c"), "a\\.b\\*c");
    });
    it("Escapes all special characters", () => {
        Exception.assertEqual(regexprEscape(".*+?^${}()|[]\\"), "\\.\\*\\+\\?\\^\\$\\{\\}\\(\\)\\|\\[\\]\\\\");
    });
    it("Can be used to match a string literally", () => {
        Exception.assert(new RegExp(regexprEscape("a.b*c")).test("a.b*c"), "Escaped regex must match literally");
    });
});