import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";
import Record from "#bzd/apps/artifacts/plugins/nodes/record.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "record");

describe("Record", () => {
	describe("Empty", () => {
		const fs = new Filesystem();
		const record = new Record("records", { fs: fs });

		it("init", async () => {
			await record.init();
		});

		it("add", async () => {
			await record.add(0);
		});

		it("add many", async () => {
			await Promise.all([record.add(1), record.add(2), record.add(3), record.add(4), record.add(5)]);
		});
	});

	describe("Existing", () => {
		const fs = new Filesystem({
			"records/0000002.rec": "abc",
			"records/0000001.rec": "a",
			"records/0000010.rec": "abcde",
			"records/invalid.rec": "abcdejsjs",
			"records/0000007.rec": "abcd",
		});
		const record = new Record("records", { fs: fs });

		it("init", async () => {
			await record.init();

			const files = await fs.readdir("records");
			Exception.assertEqual(files, ["0000002.rec", "0000001.rec", "0000010.rec", "0000007.rec"]);
			Exception.assertEqual(record.entries, [
				{ timestamp: 1, path: "records/0000001.rec", size: 1 },
				{ timestamp: 2, path: "records/0000002.rec", size: 3 },
				{ timestamp: 7, path: "records/0000007.rec", size: 4 },
				{ timestamp: 10, path: "records/0000010.rec", size: 5 },
			]);
		});
	});
});
