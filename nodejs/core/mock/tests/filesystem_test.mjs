import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";

const Exception = ExceptionFactory("test", "filesystem");

describe("Filesystem", () => {
	describe("readdir", () => {
		it("Empty", async () => {
			const fs = new Filesystem();
			Exception.assertEqual(await fs.readdir(""), []);
		});
		it("With data", async () => {
			const fs = new Filesystem({
				"hello/file1": "a",
				"hello/file2": "sas",
				"hello/dir1/file3": "dsd",
			});
			Exception.assertEqual(await fs.readdir("hello"), ["file1", "file2", "dir1"]);
		});
	});
});
