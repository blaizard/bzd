import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";
import Record from "#bzd/apps/artifacts/plugins/nodes/record.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "record");

const readAll = async (record, tick) => {
	let values = [];
	for await (const [_, value] of record.read(tick)) {
		values.push(value);
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
			await record.sanitize();
		});
	});

	describe("Existing", () => {
		const fs = new Filesystem({
			"records/2.rec": '[2,"abc"],',
			"records/1.rec": '[1,"a"],',
			"records/4.rec": '[4,"abcde"],',
			"records/5.rec": '[5,"abcdef"],',
			"records/invalid.rec": '[3,"abcdejsjs"],',
			"records/3.rec": '[3,"abcd"],',
		});
		const record = new Record("records", { fs: fs });

		it("init", async () => {
			await record.init();

			const files = await fs.readdir("records");
			Exception.assertEqual(files, ["2.rec", "1.rec", "4.rec", "5.rec", "3.rec"]);
			Exception.assertEqual(record.records, [
				{ tick: 1, path: "records/1.rec", size: 8 },
				{ tick: 2, path: "records/2.rec", size: 10 },
				{ tick: 3, path: "records/3.rec", size: 11 },
				{ tick: 4, path: "records/4.rec", size: 12 },
				{ tick: 5, path: "records/5.rec", size: 13 },
			]);
		});

		it("read", async () => {
			Exception.assertEqual((await record.read().next()).value, [1, "a"]);
			Exception.assertEqual((await record.read(1).next()).value, [1, "a"]);
			Exception.assertEqual((await record.read(2).next()).value, [2, "abc"]);
			Exception.assertEqual((await record.read(3).next()).value, [3, "abcd"]);
			Exception.assertEqual((await record.read(4).next()).value, [4, "abcde"]);
			Exception.assertEqual((await record.read(5).next()).value, [5, "abcdef"]);
			Exception.assertEqual((await record.read(6).next()).value, null);
		});

		it("read all", async () => {
			Exception.assertEqual(await readAll(record, -1), ["a", "abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 0), ["a", "abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 1), ["a", "abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 2), ["abc", "abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 3), ["abcd", "abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 4), ["abcde", "abcdef"]);
			Exception.assertEqual(await readAll(record, 5), ["abcdef"]);
			Exception.assertEqual(await readAll(record, 6), []);
			Exception.assertEqual(await readAll(record, 12), []);
			Exception.assertEqual(await readAll(record, 327327), []);
			Exception.assertEqual(await readAll(record, null), []);
		});

		it("write", async () => {
			for (let i = 0; i < 10; ++i) {
				await record.write("new" + i);
			}
		});

		it("read", async () => {
			Exception.assertEqual((await record.read(5).next()).value, [5, "abcdef"]);
			Exception.assertEqual((await record.read(6).next()).value, [6, "new0"]);
			Exception.assertEqual((await record.read(7).next()).value, [7, "new1"]);
			Exception.assertEqual((await record.read(15).next()).value, [15, "new9"]);
			Exception.assertEqual((await record.read(16).next()).value, null);
		});

		it("sanitize", async () => {
			await record.sanitize();
		});
	});

	describe("Stress", () => {
		const fs = new Filesystem();
		const record = new Record("records", {
			fs: fs,
			recordMaxSize: 100,
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

		it("sanitize", async () => {
			await record.sanitize();
		});

		it("read one by one", async () => {
			for (let tick = 1; tick < 100; ++tick) {
				const result = await record.read(tick).next();
				Exception.assertEqual(result.value[0], tick);
			}
		});
	});

	describe("paylaod larger than recordMaxSize", () => {
		const fs = new Filesystem();
		const record = new Record("records", {
			fs: fs,
			recordMaxSize: 10,
		});

		it("init", async () => {
			await record.init();
		});

		it("write", async () => {
			for (let i = 0; i < 10; ++i) {
				await record.write("ababdjksdskhfjhfjjksahdjkdgdgdgshgdahdsdklsjksjkdsashdashkdsjhdhksjhdkjhasjkdhshgds");
			}
		});

		it("sanitize", async () => {
			await record.sanitize();
		});
	});

	describe("MaxSize restriction", () => {
		const fs = new Filesystem();
		const record = new Record("records", {
			fs: fs,
			recordMaxSize: 100,
			maxSize: 300,
		});

		it("init", async () => {
			await record.init();
		});

		it("write", async () => {
			for (let i = 0; i < 100; ++i) {
				await record.write([i, "ababdjksdskhfjhfjjksahdjkdgdgdgshgdahdsdklsjksjkdsashdashkdsjhdhksjhdkjhasjkdhshgds"]);
			}
		}).timeout(10000);

		it("sanitize", async () => {
			await record.sanitize();
		});

		it("check deleted records", async () => {
			const files = await fs.readdir("records");
			Exception.assertEqual(files.length, 3);
		});
	});
});
