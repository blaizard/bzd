import Os from "os";

import ExceptionFactory from "../exception.mjs";
import LockFile from "../lock_file.mjs";

const Exception = ExceptionFactory("test", "lockFile");

describe("LockFile", () => {
	const tmpDir = process.env.TEST_TMPDIR || Os.tmpdir();
	const lockFilePath = tmpDir + "/simple_lockfile_test.lock";

	it("Simple", async () => {
		const lockFile = new LockFile(lockFilePath);
		Exception.assert(!lockFile.isLock());
		Exception.assertEqual(await lockFile.getStatus(), LockFile.Status.unlocked);

		try {
			const isLocked = await lockFile.tryLock();
			Exception.assert(isLocked);
			Exception.assert(lockFile.isLock());
			Exception.assertEqual(await lockFile.getStatus(), LockFile.Status.locked);

			await Exception.assertThrowsWithMatch(async () => await lockFile.tryLock(), "already acquired");
		} finally {
			await lockFile.unlock();
		}

		Exception.assert(!lockFile.isLock());
		let executed = false;
		await lockFile.lock(async () => {
			Exception.assert(lockFile.isLock());
			Exception.assertEqual(await lockFile.getStatus(), LockFile.Status.locked);
			executed = true;
		});
		Exception.assert(!lockFile.isLock());
		Exception.assertEqual(await lockFile.getStatus(), LockFile.Status.unlocked);
		Exception.assert(executed);
	});

	it("Concurrent", async () => {
		let result = [];
		const worker = async (uid) => {
			const lockFile = new LockFile(lockFilePath);
			await lockFile.lock(async () => {
				result.push(uid);
				await new Promise((resolve) => setTimeout(resolve, 1));
				Exception.assertEqual(result[result.length - 1], uid);
			});
		};
		const nbPromise = 50;
		const promises = [];
		for (let i = 0; i < nbPromise; i++) {
			promises.push(worker(i));
		}
		await Promise.all(promises);
		Exception.assertEqual(result.length, nbPromise);
	});
});
