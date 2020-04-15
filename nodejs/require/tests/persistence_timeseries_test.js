"use strict";

const Path = require("path");
const Fs = require("fs");
const Os = require("os");

const PersistenceTimeSeries = require("../persistence/timeseries.js");
const FileSystem = require("../filesystem.js");
const Log = require("../log.js")("test", "persistence-timestamp");
const Exception = require("../exception.js")("test", "persistence-timestamp");

Log.mute();

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
	const fileNameList = await FileSystem.readdir(Path.dirname(persistenceOptions.path));
	for (const i in fileNameList) {
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

				// Ignore duplicates (this needs to be fixed)
				if (sampleList.indexOf(timestamp) != -1) {
					continue;
				}

				await timeseries.insert(timestamp, timestamp);
				sampleList.push(timestamp);

				// Randomly close and re-open the persistence
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
				console.log(sampleList[count], timestamp);
				Exception.assertEqual(timestamp, data);
				Exception.assertEqual(sampleList[count], timestamp);
				++count;
			});

			Exception.assertEqual(count, sampleList.length);

			await timeseries.close();
		});
	});
});
