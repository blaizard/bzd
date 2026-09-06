import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import FileSystemMock from "#bzd/nodejs/core/mock/filesystem.js";
import Locks from "#bzd/apps/artifacts/backend/locks.js";
import Records from "#bzd/apps/artifacts/plugins/nodes/records/records.js";
import RecordsDistributed from "#bzd/apps/artifacts/plugins/nodes/records/distributed.js";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "records", "distributed");

const makeRecordFile = (version, tick) => {
	return '{"version":' + version + ',"records":[[' + tick + ',"x",3,null],';
};

describe("RecordsDistributed", () => {
	describe("init", () => {
		it("picks the directory with the lowest name", async () => {
			const fs = new FileSystemMock({
				"records/bbb/main/200.rec": makeRecordFile(Records.version, 200),
				"records/aaa/main/100.rec": makeRecordFile(Records.version, 100),
			});
			const records = new RecordsDistributed({
				path: "records",
				fs: fs,
				locks: new Locks("/locks", { fs: fs }),
				storages: ["main"],
			});
			await records.init({ addStopProcess: () => {} });

			Exception.assertEqual(records.records.options.path, "records/aaa");
			Exception.assertEqual((await records.records.read(0).next()).value, [100, "x", 3, null]);
		});

		it("skips a locked directory and picks the next one", async () => {
			const fs = new FileSystemMock({
				"records/aaa/main/100.rec": makeRecordFile(Records.version, 100),
				"records/bbb/main/200.rec": makeRecordFile(Records.version, 200),
			});
			const locks = new Locks("/locks", { fs: fs });
			const locked = await locks.tryLock("records/aaa");
			Exception.assert(locked !== null);

			const records = new RecordsDistributed({
				path: "records",
				fs: fs,
				locks: locks,
				storages: ["main"],
			});
			await records.init({ addStopProcess: () => {} });

			Exception.assertEqual(records.records.options.path, "records/bbb");
		});

		it("creates a new directory when the root is empty", async () => {
			const fs = new FileSystemMock();
			const records = new RecordsDistributed({
				path: "records",
				fs: fs,
				locks: new Locks("/locks", { fs: fs }),
				storages: ["main"],
			});
			await records.init({ addStopProcess: () => {} });

			const entries = await fs.readdir("records");
			Exception.assertEqual(entries.length, 1);
			Exception.assert(records.records.options.path.startsWith("records/"));
		});
	});
});
