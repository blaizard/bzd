import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "records");

const readAll = async (record, tick) => {
	let values = [];
	for await (const [_, value] of record.read(tick)) {
		values.push(value);
	}
	return values;
};

describe("Records", () => {
	describe("Empty", () => {
		const fs = new Filesystem();
		const records = new Records({ path: "records", fs: fs });

		it("init", async () => {
			await records.init();
		});

		it("write", async () => {
			await records.write(0);
		});

		it("write many", async () => {
			await Promise.all([records.write(1), records.write(2), records.write(3), records.write(4), records.write(5)]);
		});

		it("read all", async () => {
			Exception.assertEqual(new Set(await readAll(records, 0)), new Set([1, 2, 3, 4, 5]));
		});

		it("sanity check", async () => {
			await records.sanitize();
		});
	});

	describe("Existing", () => {
		const fs = new Filesystem({
			"records/main/2.rec": '{"version":1,"records":[[2,"abc",5],',
			"records/main/1.rec": '{"version":1,"records":[[1,"a",3],',
			"records/main/4.rec": '{"version":1,"records":[[4,"abcde",7],',
			"records/main/6.rec": '{"version":0,"records":[[4,"abcde",7],',
			"records/main/5.rec": '{"version":1,"records":[[5,"abcdef",8],',
			"records/main/invalid.rec": '{"version":1,"records":[[3,"abcdejsjs",10],',
			"records/main/8.rec": '{"version":1,"records":[[8,"abcdefg",9],',
			"records/main/3.rec": '{"version":1,"records":[[3,"abcd",6],',
		});
		const records = new Records({ path: "records", fs: fs });

		it("init", async () => {
			await records.init();

			const files = await fs.readdir("records/main");
			Exception.assertEqual(files, ["2.rec", "1.rec", "4.rec", "5.rec", "8.rec", "3.rec"]);
			Exception.assertEqual(records.storages["main"].records, [
				{ tick: 1, path: "records/main/1.rec", size: 34 },
				{ tick: 2, path: "records/main/2.rec", size: 36 },
				{ tick: 3, path: "records/main/3.rec", size: 37 },
				{ tick: 4, path: "records/main/4.rec", size: 38 },
				{ tick: 5, path: "records/main/5.rec", size: 39 },
				{ tick: 8, path: "records/main/8.rec", size: 40 },
			]);
		});

		it("read", async () => {
			Exception.assertEqual((await records.read().next()).value, [1, "a", 3]);
			Exception.assertEqual((await records.read(1).next()).value, [1, "a", 3]);
			Exception.assertEqual((await records.read(2).next()).value, [2, "abc", 5]);
			Exception.assertEqual((await records.read(3).next()).value, [3, "abcd", 6]);
			Exception.assertEqual((await records.read(4).next()).value, [4, "abcde", 7]);
			Exception.assertEqual((await records.read(5).next()).value, [5, "abcdef", 8]);
			Exception.assertEqual((await records.read(6).next()).value, [8, "abcdefg", 9]);
			Exception.assertEqual((await records.read(7).next()).value, [8, "abcdefg", 9]);
			Exception.assertEqual((await records.read(8).next()).value, [8, "abcdefg", 9]);
			Exception.assertEqual((await records.read(9).next()).value, null);
		});

		it("read all", async () => {
			Exception.assertEqual(await readAll(records, -1), ["a", "abc", "abcd", "abcde", "abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 0), ["a", "abc", "abcd", "abcde", "abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 1), ["a", "abc", "abcd", "abcde", "abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 2), ["abc", "abcd", "abcde", "abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 3), ["abcd", "abcde", "abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 4), ["abcde", "abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 5), ["abcdef", "abcdefg"]);
			Exception.assertEqual(await readAll(records, 6), ["abcdefg"]);
			Exception.assertEqual(await readAll(records, 7), ["abcdefg"]);
			Exception.assertEqual(await readAll(records, 8), ["abcdefg"]);
			Exception.assertEqual(await readAll(records, 9), []);
			Exception.assertEqual(await readAll(records, 12), []);
			Exception.assertEqual(await readAll(records, 327327), []);
			Exception.assertEqual(await readAll(records, null), []);
		});

		it("write", async () => {
			for (let i = 0; i < 10; ++i) {
				await records.write("new" + i);
			}
		});

		it("read", async () => {
			Exception.assertEqual((await records.read(8).next()).value, [8, "abcdefg", 9]);
			Exception.assertEqual((await records.read(9).next()).value, [9, "new0", 6]);
			Exception.assertEqual((await records.read(10).next()).value, [10, "new1", 6]);
			Exception.assertEqual((await records.read(18).next()).value, [18, "new9", 6]);
			Exception.assertEqual((await records.read(19).next()).value, null);
		});

		it("sanitize", async () => {
			await records.sanitize();
		});
	});

	describe("Stress", () => {
		const fs = new Filesystem();
		const records = new Records({
			path: "records",
			fs: fs,
			recordMaxSize: 100,
		});

		it("init", async () => {
			await records.init();
		});

		it("write many", async () => {
			let counter = 0;
			for (let i = 0; i < 100; ++i) {
				await Promise.all([
					records.write(++counter),
					records.write(++counter),
					records.write(++counter),
					records.write(++counter),
					records.write(++counter),
				]);
			}
		}).timeout(10000);

		it("sanitize", async () => {
			await records.sanitize();
		});

		it("read one by one", async () => {
			for (let tick = 1; tick < 100; ++tick) {
				const result = await records.read(tick).next();
				Exception.assertEqual(result.value[0], tick);
			}
		});
	});

	describe("paylaod larger than recordMaxSize", () => {
		const fs = new Filesystem();
		const records = new Records({
			path: "records",
			fs: fs,
			recordMaxSize: 10,
		});

		it("init", async () => {
			await records.init();
		});

		it("write", async () => {
			for (let i = 0; i < 10; ++i) {
				await records.write("ababdjksdskhfjhfjjksahdjkdgdgdgshgdahdsdklsjksjkdsashdashkdsjhdhksjhdkjhasjkdhshgds");
			}
		});

		it("sanitize", async () => {
			await records.sanitize();
		});
	});

	describe("MaxSize restriction", () => {
		const fs = new Filesystem();
		const records = new Records({
			path: "records",
			fs: fs,
			recordMaxSize: 100,
			maxSize: 300,
		});

		it("init", async () => {
			await records.init();
		});

		it("write", async () => {
			for (let i = 0; i < 100; ++i) {
				await records.write([i, "x" * 40]);
			}
		}).timeout(10000);

		it("sanitize", async () => {
			await records.sanitize();
		});

		it("check deleted records", async () => {
			const files = await fs.readdir("records");
			Exception.assert(files.length <= 6);
		});
	});
});
