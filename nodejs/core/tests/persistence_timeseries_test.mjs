import Path from "path";
import Fs from "fs";
import Os from "os";

import PersistenceTimeSeries from "../persistence/timeseries.mjs";
import FileSystem from "../filesystem.mjs";
import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("test", "persistence-timestamp");

//Log.mute();

const tempDirPath = Fs.mkdtempSync(Path.join(Os.tmpdir(), "persistence-timeseries-"));
const persistenceOptions = {
	path: Path.resolve(tempDirPath, "test.persistence.timeseries.json"),
	maxEntriesPerFile: 10,
	savepointTask: null
};

after(async () => {
	await FileSystem.rmdir(tempDirPath);
});

async function environmentCleanup() {
	const path = Path.dirname(persistenceOptions.path);
	await FileSystem.rmdir(path);
	await FileSystem.mkdir(path);
}

describe("PersistenceTimeSeries", () => {
	describe("actions", () => {
		it("increasing timestamp", async () => {
			await environmentCleanup();

			let timeseries = await PersistenceTimeSeries.make(persistenceOptions.path, persistenceOptions);

			// Build a list of random points to be added
			let sampleList = [];
			for (let i = 0; i < 100; ++i) {
				const timestamp = i; //Math.floor(Math.random() * Number.MAX_VALUE - Number.MAX_VALUE / 2);
				await timeseries.insert(timestamp, timestamp);
				sampleList.push(timestamp);
			}

			// Check the samples
			{
				let count = 0;
				await timeseries.forEach((timestamp, data) => {
					Exception.assertEqual(timestamp, data);
					Exception.assertEqual(sampleList[count], timestamp);
					++count;
				});
				Exception.assertEqual(count, sampleList.length);
			}
			{
				let count = 50;
				await timeseries.forEach((timestamp, data) => {
					Exception.assertEqual(timestamp, data);
					Exception.assertEqual(sampleList[count], timestamp);
					++count;
				}, 50);
				Exception.assertEqual(count, sampleList.length);
			}
			{
				let count = 50;
				await timeseries.forEach(
					(timestamp, data) => {
						Exception.assertEqual(timestamp, data);
						Exception.assertEqual(sampleList[count], timestamp);
						++count;
					},
					50,
					80
				);
				Exception.assertEqual(count, 81);
			}

			await timeseries.close();
		}).timeout(30000);

		it("random timestamp", async () => {
			await environmentCleanup();

			let timeseries = await PersistenceTimeSeries.make(persistenceOptions.path, persistenceOptions);

			// Build a list of random points to be added
			let sampleList = [];
			for (let i = 0; i < 100; ++i) {
				const timestamp = Math.floor(Math.random() * 1000); // - 500);

				// Ignore duplicates (this needs to be fixed)
				if (sampleList.indexOf(timestamp) != -1) {
					continue;
				}

				await timeseries.insert(timestamp, timestamp);
				sampleList.push(timestamp);

				// Randomly close and re-open the persistence
				if (Math.floor(Math.random() * 10) == 0) {
					await timeseries.close();
					timeseries = await PersistenceTimeSeries.make(persistenceOptions.path, persistenceOptions);
				}
			}

			// Check the samples
			let count = 0;
			sampleList.sort((a, b) => a - b);

			await timeseries.forEach((timestamp, data) => {
				Exception.assertEqual(timestamp, data);
				Exception.assertEqual(sampleList[count], timestamp);
				++count;
			});

			Exception.assertEqual(count, sampleList.length);

			await timeseries.close();
		}).timeout(30000);
	});
});
