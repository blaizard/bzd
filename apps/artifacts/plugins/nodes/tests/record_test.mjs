import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";
import Record from "#bzd/apps/artifacts/plugins/nodes/record.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "record");

const readAll = async (record, timestamp) => {
	let values = [];
	for await (const newValues of record.read(timestamp)) {
		values = [...values, ...newValues];
	}
	return values;
};

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

		it("read all", async () => {
			Exception.assertEqual(new Set(await readAll(record, 0)), new Set([1, 2, 3, 4, 5]));
		});

		it("sanity check", async () => {
			await record.sanityCheck(/*includePayload*/ true);
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
			Exception.assertEqual(record.records, [
				{ timestamp: 1, path: "records/1.rec", size: 8, next: 2 },
				{ timestamp: 2, path: "records/2.rec", size: 10, next: 7 },
				{ timestamp: 7, path: "records/7.rec", size: 11, next: 10 },
				{ timestamp: 10, path: "records/10.rec", size: 13, next: 11 },
				{ timestamp: 11, path: "records/011.rec", size: 14, next: null },
			]);
		});

		it("read", async () => {
			Exception.assertEqual((await record.read().next()).value, ["a"]);
			Exception.assertEqual((await record.read(1).next()).value, ["a"]);
			Exception.assertEqual((await record.read(2).next()).value, ["abc"]);
			Exception.assertEqual((await record.read(3).next()).value, ["abcd"]);
			Exception.assertEqual((await record.read(7).next()).value, ["abcd"]);
			Exception.assertEqual((await record.read(10).next()).value, ["abcde"]);
			Exception.assertEqual((await record.read(11).next()).value, ["abcdef"]);
			Exception.assertEqual((await record.read(12).next()).value, null);
		});

		it("read all", async () => {
			Exception.assertEqual(await readAll(record, -1), ["a", "abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 0), ["a", "abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 1), ["a", "abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 2), ["abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 3), ["abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 4), ["abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 5), ["abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 6), ["abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 7), ["abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 8), ["abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 9), ["abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 10), ["abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 11), ["abcdef"]);
			Exception.assertEqual(await readAll(record, 12), []);
			Exception.assertEqual(await readAll(record, 327327), []);
			Exception.assertEqual(await readAll(record, null), []);
		});
	});

	describe("Stress", () => {
		const fs = new Filesystem();
		const record = new Record("records", {
			fs: fs,
			recordMaxSize: 100,
			maxSize: 20 * 1024 * 1024,
		});

		it("init", async () => {
			await record.init();
		});

		it("write many", async () => {
			let counter = 0;
			for (let i = 0; i < 100; ++i) {
				await Promise.all([
					record.write(++counter),
					record.write(++counter),
					record.write(++counter),
					record.write(++counter),
					record.write(++counter),
				]);
			}
		}).timeout(10000);

		it("read", async () => {
			await record.sanityCheck(/*includePayload*/ true);
		});
	});
});
