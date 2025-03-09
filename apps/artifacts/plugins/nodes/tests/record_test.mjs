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

		it("write", async () => {
			await record.write(0);
		});

		it("write many", async () => {
			await Promise.all([record.write(1), record.write(2), record.write(3), record.write(4), record.write(5)]);
		});
	});

	describe("Existing", () => {
		const fs = new Filesystem({
			"records/2.rec": '[2,"abc"],',
			"records/1.rec": '[1,"a"],',
			"records/10.rec": '[10,"abcde"],',
			"records/011.rec": '[11,"abcdef"],',
			"records/invalid.rec": '[3,"abcdejsjs"],',
			"records/7.rec": '[7,"abcd"],',
		});
		const record = new Record("records", { fs: fs });

		it("init", async () => {
			await record.init();

			const files = await fs.readdir("records");
			Exception.assertEqual(files, ["2.rec", "1.rec", "10.rec", "011.rec", "7.rec"]);
			Exception.assertEqual(record.entries, [
				{ timestamp: 1, path: "records/1.rec", size: 8 },
				{ timestamp: 2, path: "records/2.rec", size: 10 },
				{ timestamp: 7, path: "records/7.rec", size: 11 },
				{ timestamp: 10, path: "records/10.rec", size: 13 },
				{ timestamp: 11, path: "records/011.rec", size: 14 },
			]);
		});

		it("read", async () => {
			Exception.assertEqual(await record.read(), ["a", 2]);
			Exception.assertEqual(await record.read(2), ["abc", 7]);
			Exception.assertEqual(await record.read(7), ["abcd", 10]);
			Exception.assertEqual(await record.read(10), ["abcde", 11]);
			Exception.assertEqual(await record.read(11), ["abcdef", null]);
			Exception.assertEqual(await record.read(12), null);
		});
	});
});
