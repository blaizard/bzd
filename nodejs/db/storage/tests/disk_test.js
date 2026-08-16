import Path from "path";

import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import StorageDisk from "#bzd/nodejs/db/storage/disk.js";

const Exception = ExceptionFactory("test", "db", "storage", "disk");

describe("StorageDisk", () => {
	describe("_getFullPath", () => {
		const storage = new StorageDisk("/tmp/storage");

		it("joins a simple path", () => {
			Exception.assertEqual(storage._getFullPath(["a", "b"]), Path.join("/tmp/storage", "a", "b"));
		});

		it("joins an empty path to the root", () => {
			Exception.assertEqual(storage._getFullPath([]), "/tmp/storage");
		});

		it("normalizes parent and current directory segments", () => {
			Exception.assertEqual(storage._getFullPath(["a", "..", ".", "b"]), Path.join("/tmp/storage", "b"));
		});

		it("allows segments containing spaces", () => {
			Exception.assertEqual(storage._getFullPath(["a b"]), Path.join("/tmp/storage", "a b"));
		});

		it("throws when a leading parent directory escapes beyond the root", () => {
			Exception.assertThrowsWithMatch(() => {
				storage._getFullPath([".."]);
			}, "expands beyond the root");
		});

		it("throws when parent directories expand beyond the root", () => {
			Exception.assertThrowsWithMatch(() => {
				storage._getFullPath(["a", "..", ".."]);
			}, "expands beyond the root");
		});

		it("throws when a segment contains a path separator", () => {
			Exception.assertThrowsWithMatch(() => {
				storage._getFullPath(["../../etc/passwd"]);
			}, "path separator");
		});

		it("throws when a segment contains a path separator within a larger path", () => {
			Exception.assertThrowsWithMatch(() => {
				storage._getFullPath(["a/../b"]);
			}, "path separator");
		});

		it("throws when a segment contains a parent directory reference", () => {
			Exception.assertThrowsWithMatch(() => {
				storage._getFullPath(["../x"]);
			}, "path separator");
		});
	});
});
