import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Filesystem from "#bzd/nodejs/core/mock/filesystem.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records.mjs";
import RecordsReader from "#bzd/apps/artifacts/plugins/nodes/records_reader.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "records-reader");

describe("RecordsReader", () => {
	describe("Static", () => {
		const fs = new Filesystem({
			"records/main/2.rec": '{"version":2,"records":[[2,"abc",5,null],',
			"records/main/1.rec": '{"version":2,"records":[[1,"a",3,null],',
			"records/main/4.rec": '{"version":2,"records":[[4,"abcde",7,null],',
			"records/main/6.rec": '{"version":1,"records":[[4,"abcde",7],',
			"records/main/invalid.rec": '{"version":2,"records":[[3,"abcdejsjs",10,null],',
			"records/main/8.rec": '{"version":2,"records":[[8,"abcdefg",9,12],',
			"records/main/3.rec": '{"version":2,"records":[[3,"abcd",6,null],',
		});
		const recordsReader = new RecordsReader({ path: "records", fs: fs });

		it("init", async () => {
			await recordsReader.init();
			Exception.assertEqual(recordsReader.storages, {
				main: {
					records: [
						{ tick: 1, path: "records/main/1.rec", hash: 39 },
						{ tick: 2, path: "records/main/2.rec", hash: 41 },
						{ tick: 3, path: "records/main/3.rec", hash: 42 },
						{ tick: 4, path: "records/main/4.rec", hash: 43 },
						{ tick: 6, path: "records/main/6.rec", hash: 38 },
						{ tick: 8, path: "records/main/8.rec", hash: 43 },
					],
					hash: "6.24",
				},
			});
		});
	});

	describe("Concurrent", () => {
		const fs = new Filesystem();
		const records = new Records({ path: "records", fs: fs, recordMaxSize: 100, maxSize: 1000 });
		const recordsReader = new RecordsReader({ path: "records", fs: fs });

		it("init", async () => {
			await records.init();
			await recordsReader.init();
		});

		it("concurrent", async () => {
			let counter = 0;
			let readChunkLastCounter = 0;
			let readChunkFirstCounter = 0;

			const writer = async () => {
				const value = ++counter;
				await records.write(value);
			};

			const readChunkLast = async () => {
				let previousValue = 0;
				for await (const [_, value] of recordsReader.read(counter - 3)) {
					if (previousValue) {
						Exception.assertEqual(value, previousValue + 1);
					}
					previousValue = value;
					++readChunkLastCounter;
				}
			};

			const readChunkFirst = async () => {
				let previousValue = 0;
				let count = 0;
				for await (const [_, value] of recordsReader.read(0)) {
					if (previousValue) {
						Exception.assertEqual(value, previousValue + 1);
					}
					previousValue = value;
					++count;
					++readChunkFirstCounter;
					if (count == 5) {
						break;
					}
				}
			};

			for (let i = 0; i < 100; ++i) {
				await Promise.all([writer(), readChunkLast(), writer(), readChunkFirst()]);
			}

			Exception.assert(readChunkLastCounter > 0);
			Exception.assert(readChunkFirstCounter > 0);
		}).timeout(10000);
	});
});
