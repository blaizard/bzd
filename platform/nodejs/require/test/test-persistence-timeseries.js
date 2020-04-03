"use strict";

const Path = require("path");

const PersistenceTimeSeries = require("../persistence/timeseries.js");
const FileSystem = require("../filesystem.js");
const Log = require("../log.js")("test", "persistence-timestamp");
const Exception = require("../exception.js")("test", "persistence-timestamp");

Log.mute();

const persistenceOptions = {
	path: Path.resolve(__dirname, "test-persistence-timeseries", "test.persistence.timeseries.json"),
	maxEntriesPerFile: 10,
	savepointTask: null
};

async function environmentCleanup() {
	const fileNameList = await FileSystem.readdir(Path.dirname(persistenceOptions.path));
	for (const i in fileNameList) {
		if (fileNameList[i] == ".gitignore") {
			continue;
		}
		const filePath = Path.join(Path.dirname(persistenceOptions.path), fileNameList[i]);
		if ((await FileSystem.stat(filePath)).isDirectory()) {
			await FileSystem.rmdir(filePath);
		}
		else {
			await FileSystem.unlink(filePath);
		}
	}
}

describe("PersistenceTimeSeries", () => {

	describe("actions", () => {

		it("increasing timestamp", async () => {

			await environmentCleanup();

			let timeseries = new PersistenceTimeSeries(persistenceOptions.path, persistenceOptions);
			await timeseries.waitReady();

			// Build a list of random points to be added
			let sampleList = [];
			for (let i = 0; i<100; ++i) {
				const timestamp = i; //Math.floor(Math.random() * Number.MAX_VALUE - Number.MAX_VALUE / 2);
				await timeseries.insert(timestamp, timestamp);
				sampleList.push(timestamp);
			}

			// Check the samples
			let count = 0;
			await timeseries.forEach((timestamp, data) => {
				Exception.assertEqual(timestamp, data);
				Exception.assertEqual(sampleList[count], timestamp);
				++count;
			});

			Exception.assertEqual(count, sampleList.length);

			await timeseries.close();
		});

		it("random timestamp", async () => {

			await environmentCleanup();

			let timeseries = new PersistenceTimeSeries(persistenceOptions.path, persistenceOptions);
			await timeseries.waitReady();

			// Build a list of random points to be added
			let sampleList = [];
			for (let i = 0; i<100; ++i) {
				const timestamp = Math.floor(Math.random() * 1000 - 500);
				await timeseries.insert(timestamp, timestamp);
				sampleList.push(timestamp);

				// Randmly close and re-open the persistence
				if (Math.floor(Math.random() * 10) == 0) {
					await timeseries.close();
					timeseries = new PersistenceTimeSeries(persistenceOptions.path, persistenceOptions);
					await timeseries.waitReady();
				}
			}

			// Check the samples
			let count = 0;
			sampleList.sort((a, b) => (a - b));

			await timeseries.forEach((timestamp, data) => {
				Exception.assertEqual(timestamp, data);
				Exception.assertEqual(sampleList[count], timestamp);
				++count;
			});

			Exception.assertEqual(count, sampleList.length);

			await timeseries.close();
		});
	});
});
