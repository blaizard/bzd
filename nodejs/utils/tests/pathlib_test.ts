import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import pathlib from "#bzd/nodejs/utils/pathlib.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "pathlib");

describe("pathlib", () => {
    describe("path", () => {
        it("Parses relative and absolute paths", () => {
            Exception.assertEqual(pathlib.path("a/b/c").asPosix(), "a/b/c");
            Exception.assertEqual(pathlib.path("/a/b/c").asPosix(), "/a/b/c");
        });
        it("Filters empty segments", () => {
            Exception.assertEqual(pathlib.path("a//b/").asPosix(), "a/b");
        });
        it("Accepts an array of segments", () => {
            Exception.assertEqual(pathlib.path(["a", "b"]).asPosix(), "a/b");
        });
        it("Rejects a segment containing a separator", () => {
            Exception.assertThrows(() => pathlib.path(["a/b"]));
        });
        it("Returns a Path instance as-is", () => {
            const path = pathlib.path("a/b");
            Exception.assertEqual(pathlib.path(path), path);
        });
    });

    describe("name", () => {
        it("Returns the final component", () => {
            Exception.assertEqual(pathlib.path("a/b/c.txt").name, "c.txt");
        });
    });

    describe("suffix", () => {
        it("Returns the extension of the final component", () => {
            Exception.assertEqual(pathlib.path("a/b/c.txt").suffix, "txt");
        });
    });

    describe("parent", () => {
        it("Returns the parent path", () => {
            Exception.assertEqual(pathlib.path("a/b/c").parent.asPosix(), "a/b");
        });
    });

    describe("parts", () => {
        it("Returns the path components", () => {
            Exception.assertEqual(pathlib.path("a/b/c").parts, ["a", "b", "c"]);
        });
        it("Includes the root for absolute paths", () => {
            Exception.assertEqual(pathlib.path("/a/b").parts, ["/", "a", "b"]);
        });
    });

    describe("normalize", () => {
        it("Simple path remains unchanged", () => {
            Exception.assertEqual(pathlib.path("a/b/c").normalize.asPosix(), "a/b/c");
        });
        it("Parent references are resolved", () => {
            Exception.assertEqual(pathlib.path("a/b/../c").normalize.asPosix(), "a/c");
        });
        it("Current directory references are removed", () => {
            Exception.assertEqual(pathlib.path("a/./b").normalize.asPosix(), "a/b");
        });
        it("Dangerous '..' throws", () => {
            Exception.assertThrows(() => pathlib.path("..").normalize);
        });
    });

    describe("clone", () => {
        it("Returns an independent copy", () => {
            const original = pathlib.path("a/b");
            const clone = original.clone().joinPath("c");
            Exception.assertEqual(original.asPosix(), "a/b");
            Exception.assertEqual(clone.asPosix(), "a/b/c");
        });
    });

    describe("absolute", () => {
        it("Leaves an absolute path unchanged", () => {
            Exception.assertEqual(pathlib.path("/a/b").absolute().asPosix(), "/a/b");
        });
    });

    describe("asUri", () => {
        it("Returns a file URI for absolute paths", () => {
            Exception.assertEqual(pathlib.path("/a/b").asUri(), "file:///a/b");
        });
        it("Throws for relative paths", () => {
            Exception.assertThrows(() => pathlib.path("a/b").asUri());
        });
    });

    describe("joinPath", () => {
        it("Joins segments", () => {
            Exception.assertEqual(pathlib.path("a").joinPath("b", "c").asPosix(), "a/b/c");
        });
        it("Joins with a Path instance", () => {
            Exception.assertEqual(pathlib.path("a").joinPath(pathlib.path("b/c")).asPosix(), "a/b/c");
        });
    });

    describe("tmp", () => {
        it("Returns the absolute temporary directory", () => {
            Exception.assertEqual(pathlib.tmp().isAbsolute(), true);
        });
    });
});