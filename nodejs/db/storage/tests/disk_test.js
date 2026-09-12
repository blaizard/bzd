import Path from "path";

import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.js";
import StorageDisk from "#bzd/nodejs/db/storage/disk.js";

const Exception = ExceptionFactory("test", "db", "storage", "disk");

describe("StorageDisk", () => {
	describe("_getFullPath", () => {
		let storage = null;

		before(async () => {
			storage = await StorageDisk.make("root", { fs: new Filesystem(), write: true });
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
			const filesystem = new Filesystem({
				"root/inside.txt": "inside",
				"root/secret.txt": "secret",
			});
			// A symlink inside the root pointing to a file outside of the root.
			await filesystem.symlink("/etc/passwd", "root/escape-file.txt");
			// A symlink inside the root pointing to a directory outside of the root.
			await filesystem.symlink("/etc", "root/escape-dir");
			storage = await StorageDisk.make("root", { fs: filesystem, write: true });
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
			const filesystem = new Filesystem({
				"real-root/inside.txt": "inside",
			});
			await filesystem.symlink("/etc/passwd", "real-root/escape-file.txt");
			await filesystem.symlink("real-root", "link-root");
			storage = await StorageDisk.make("link-root", { fs: filesystem, write: true });
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
