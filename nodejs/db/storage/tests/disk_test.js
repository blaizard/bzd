import Fs from "fs";
import Os from "os";
import Path from "path";

import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import StorageDisk from "#bzd/nodejs/db/storage/disk.js";

const Exception = ExceptionFactory("test", "db", "storage", "disk");

describe("StorageDisk", () => {
	describe("_getFullPath", () => {
		let storage = null;

		before(async () => {
			const root = await Fs.promises.mkdtemp(Path.join(Os.tmpdir(), "bzd-disk-"));
			storage = await StorageDisk.make(root, { write: true });
		});

		it("joins a simple path", async () => {
			Exception.assertEqual(await storage._getFullPath(["a", "b"]), Path.join(storage.path, "a", "b"));
		});

		it("joins an empty path to the root", async () => {
			Exception.assertEqual(await storage._getFullPath([]), storage.path);
		});

		it("normalizes parent and current directory segments", async () => {
			Exception.assertEqual(await storage._getFullPath(["a", "..", ".", "b"]), Path.join(storage.path, "b"));
		});

		it("allows segments containing spaces", async () => {
			Exception.assertEqual(await storage._getFullPath(["a b"]), Path.join(storage.path, "a b"));
		});

		it("throws when a leading parent directory escapes beyond the root", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage._getFullPath([".."]);
			}, "expands beyond the root");
		});

		it("throws when parent directories expand beyond the root", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage._getFullPath(["a", "..", ".."]);
			}, "expands beyond the root");
		});

		it("throws when a segment contains a path separator", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage._getFullPath(["../../etc/passwd"]);
			}, "path separator");
		});

		it("throws when a segment contains a path separator within a larger path", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage._getFullPath(["a/../b"]);
			}, "path separator");
		});

		it("throws when a segment contains a parent directory reference", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage._getFullPath(["../x"]);
			}, "path separator");
		});
	});

	describe("symlink escape prevention", () => {
		let storage = null;

		before(async () => {
			const root = await Fs.promises.mkdtemp(Path.join(Os.tmpdir(), "bzd-disk-"));
			await Fs.promises.writeFile(Path.join(root, "inside.txt"), "inside");
			await Fs.promises.writeFile(Path.join(root, "secret.txt"), "secret");
			// A symlink inside the root pointing to a file outside of the root.
			await Fs.promises.symlink("/etc/passwd", Path.join(root, "escape-file.txt"));
			// A symlink inside the root pointing to a directory outside of the root.
			await Fs.promises.symlink("/etc", Path.join(root, "escape-dir"));
			storage = await StorageDisk.make(root, { write: true });
		});

		it("reads a file inside the root", async () => {
			Exception.assertEqual(await storage.readToString(["inside.txt"]), "inside");
		});

		it("rejects reading a file through a symlink escaping the root", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage.readToString(["escape-file.txt"]);
			}, "outside of the storage root");
		});

		it("rejects listing through a symlinked directory escaping the root", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage.list(["escape-dir"]);
			}, "outside of the storage root");
		});

		it("rejects writing through a symlinked directory escaping the root", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage.writeFromChunk(["escape-dir", "new.txt"], "new");
			}, "outside of the storage root");
		});
	});

	describe("symlinked storage root", () => {
		let storage = null;

		before(async () => {
			const realRoot = await Fs.promises.mkdtemp(Path.join(Os.tmpdir(), "bzd-disk-real-"));
			await Fs.promises.writeFile(Path.join(realRoot, "inside.txt"), "inside");
			await Fs.promises.symlink("/etc/passwd", Path.join(realRoot, "escape-file.txt"));
			const root = await Fs.promises.mkdtemp(Path.join(Os.tmpdir(), "bzd-disk-link-"));
			await Fs.promises.rmdir(root);
			await Fs.promises.symlink(realRoot, root);
			storage = await StorageDisk.make(root, { write: true });
		});

		it("reads a file through a symlinked root", async () => {
			Exception.assertEqual(await storage.readToString(["inside.txt"]), "inside");
		});

		it("rejects reading a file through a symlink escaping the root", async () => {
			await Exception.assertThrowsWithMatch(async () => {
				await storage.readToString(["escape-file.txt"]);
			}, "outside of the storage root");
		});
	});
});
