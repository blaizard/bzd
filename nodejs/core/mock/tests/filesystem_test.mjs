import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";

const Exception = ExceptionFactory("test", "filesystem");

describe("Filesystem", () => {
	describe("readdir", () => {
		it("readdir", async () => {
			{
				const fs = new Filesystem();
				Exception.assertEqual(await fs.readdir(""), []);
				Exception.assertThrows(async () => await fs.readdir("no"));
			}
			{
				const fs = new Filesystem({
					"hello/file1": "a",
					"hello/file2": "sas",
					"hello/dir1/file3": "dsd",
				});
				Exception.assertEqual(await fs.readdir(""), ["hello"]);
				Exception.assertEqual(await fs.readdir("hello"), ["file1", "file2", "dir1"]);
			}
		});

		it("mkdir", async () => {
			const fs = new Filesystem({
				"hello/dir1/file3": "dsd",
			});
			await fs.mkdir("world");
			Exception.assertEqual(await fs.readdir(""), ["hello", "world"]);
			await fs.mkdir("hello/dir1/dir2");
			Exception.assertEqual(await fs.readdir("hello/dir1"), ["file3", "dir2"]);
			Exception.assertThrows(async () => await fs.mkdir("hello/dir1/file3"));

			await fs.mkdir("hello/dir1/dir2", { force: true });
			Exception.assertThrows(async () => await fs.mkdir("hello/dir1/dir2", { force: false }));
		});

		it("rmdir", async () => {
			const fs = new Filesystem({
				"hello/dir1/file3": "dsd",
				"hello/dir2/file4": "ds",
			});
			Exception.assertThrows(async () => await fs.rmdir("hello/dir1/file3"));
			await fs.rmdir("hello/dir1");
			Exception.assertEqual(await fs.readdir("hello"), ["dir2"]);
		});

		it("readFile", async () => {
			const fs = new Filesystem({
				"hello/file1": "adds",
			});
			Exception.assertEqual(await fs.readFile("hello/file1"), "adds");
			Exception.assertThrows(async () => await fs.readFile("hello/notexist"));
			Exception.assertThrows(async () => await fs.readFile("hello"));
		});

		it("appendFile", async () => {
			const fs = new Filesystem({
				"hello/file1": "a",
			});
			await fs.appendFile("hello/file1", "bc");
			Exception.assertEqual(await fs.readFile("hello/file1"), "abc");
			await fs.appendFile("hello/file2", "new");
			Exception.assertEqual(await fs.readFile("hello/file2"), "new");
			Exception.assertThrows(async () => await fs.appendFile("hello", "a directory"));
		});
	});
});
