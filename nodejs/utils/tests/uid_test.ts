import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { makeUid } from "#bzd/nodejs/utils/uid.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "uid");

describe("makeUid", () => {
    it("Returns a string prefixed with 'uid-'", () => {
        Exception.assert(makeUid().startsWith("uid-"), "Missing uid prefix");
    });
    it("Returns unique values", () => {
        Exception.assert(makeUid() !== makeUid(), "Uids must be unique");
    });
});