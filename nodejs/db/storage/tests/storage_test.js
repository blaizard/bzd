import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import StorageMemory from "#bzd/nodejs/db/storage/memory.js";
import pathlib from "#bzd/nodejs/utils/pathlib.js";

const Exception = ExceptionFactory("test", "db", "storage");

describe("Storage", () => {
	describe("accepts a pathlib.Path", () => {
		const storage = new StorageMemory({}, { write: true });

		it("mkdir with a Path", async () => {
			await storage.mkdir(pathlib.path("a/b"));
			Exception.assertEqual(await storage.is(pathlib.path("a/b")), true);
		});

		it("write and read with a Path", async () => {
			await storage.writeFromChunk(pathlib.path("a/b/c.txt"), "content for c");
			const content = await storage.readToString(pathlib.path("a/b/c.txt"));
			Exception.assertEqual(content, "content for c");
		});

		it("list with a Path", async () => {
			const result = await storage.list(pathlib.path("a"));
			Exception.assertEqual(result.data(), ["b"]);
		});

		it("delete with a Path", async () => {
			await storage.delete(pathlib.path("a/b/c.txt"));
			Exception.assertEqual(await storage.is(pathlib.path("a/b/c.txt")), false);
		});
	});

	describe("accepts a string as a single segment", () => {
		const storage = new StorageMemory({}, { write: true });

		it("write and read a root file", async () => {
			await storage.writeFromChunk("single.txt", "content for single");
			Exception.assertEqual(await storage.is("single.txt"), true);
			Exception.assertEqual(await storage.readToString("single.txt"), "content for single");
		});
	});
});
