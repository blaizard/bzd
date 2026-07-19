import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import pathlib from "#bzd/nodejs/utils/pathlib.js";

const Exception = ExceptionFactory("test", "utils", "pathlib");

describe("pathlib", () => {
	describe("normalize", () => {
		it("Simple path remains unchanged", () => {
			Exception.assertEqual(pathlib.path("a/b/c").normalize.asPosix(), "a/b/c");
		});
		it("Parent references are resolved", () => {
			Exception.assertEqual(pathlib.path("a/b/../c").normalize.asPosix(), "a/c");
		});
		it("Dangerous '..' throws", () => {
			Exception.assertThrows(() => pathlib.path("..").normalize);
		});
	});
});
