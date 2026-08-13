import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";

const Exception = ExceptionFactory("test", "artifacts", "common");

describe("Utils", () => {
	describe("pathToKey", () => {
		it("basic path", () => {
			Exception.assertEqual(Utils.pathToKey("/foo/bar"), ["foo", "bar"]);
		});

		it("empty path", () => {
			Exception.assertEqual(Utils.pathToKey("/"), []);
		});

		it("decodes URI components", () => {
			Exception.assertEqual(Utils.pathToKey("/foo%20bar/baz"), ["foo bar", "baz"]);
		});
	});

	describe("keyToPath", () => {
		it("basic key", () => {
			Exception.assertEqual(Utils.keyToPath(["foo", "bar"]), "/foo/bar");
		});

		it("empty key", () => {
			Exception.assertEqual(Utils.keyToPath([]), "/");
		});

		it("encodes URI components", () => {
			Exception.assertEqual(Utils.keyToPath(["foo bar", "baz"]), "/foo%20bar/baz");
		});
	});

	describe("sanitizeKey", () => {
		it("keeps a simple key unchanged", () => {
			Exception.assertEqual(Utils.sanitizeKey(["a", "b", "c"]), ["a", "b", "c"]);
		});

		it("normalizes parent directory segments", () => {
			Exception.assertEqual(Utils.sanitizeKey(["a", "..", "b"]), ["b"]);
		});

		it("removes current directory segments", () => {
			Exception.assertEqual(Utils.sanitizeKey(["a", ".", "b"]), ["a", "b"]);
		});

		it("collapses redundant parent directory segments", () => {
			Exception.assertEqual(Utils.sanitizeKey(["a", "b", "..", "..", "c"]), ["c"]);
		});

		it("handles an empty key", () => {
			Exception.assertEqual(Utils.sanitizeKey([]), []);
		});

		it("handles only current directory segments", () => {
			Exception.assertEqual(Utils.sanitizeKey([".", "."]), []);
		});

		it("throws when escaping beyond the root", () => {
			Exception.assertThrowsWithMatch(() => {
				Utils.sanitizeKey(["..", ".."]);
			}, "expands beyond the root");
		});

		it("throws when a leading parent directory escapes beyond the root", () => {
			Exception.assertThrowsWithMatch(() => {
				Utils.sanitizeKey(["..", "a"]);
			}, "expands beyond the root");
		});

		it("does not modify the input key", () => {
			const key = ["a", "..", "b"];
			Utils.sanitizeKey(key);
			Exception.assertEqual(key, ["a", "..", "b"]);
		});
	});
});
