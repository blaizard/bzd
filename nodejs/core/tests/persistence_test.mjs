import Path from "path";
import Fs from "fs";
import Os from "os";
import Assert from "assert";

import PersistenceDisk from "../persistence/disk.mjs";
import FileSystem from "../filesystem.mjs";

const tempDirPath = Fs.mkdtempSync(Path.join(Os.tmpdir(), "persistence-"));

const persistenceOptions = {
	path: Path.join(tempDirPath, "test.persistence.json"),
	savepointOnClose: false,
	iterations: 100
};

let persistence = null;

before(async () => {
	persistence = await PersistenceDisk.make(persistenceOptions.path, persistenceOptions);
});

after(async () => {
	await persistence.close();
	await FileSystem.rmdir(tempDirPath);
});

describe("Persistence", function() {
	describe("actions", function() {
		it("set hello", async function() {
			await persistence.waitReady();
			await persistence.write("set", "hello", "world");
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.ok(data.hello == "world", persistence.data);
		}).timeout(10000);
		it("set mister", async function() {
			await persistence.write("set", "mister", ["a"]);
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.ok(data.mister instanceof Array, data);
			Assert.ok(data.mister[0] == "a", data);
		}).timeout(10000);
		it("delete", async function() {
			await persistence.write("delete", "hello");
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.ok(data.hello == undefined, data);
		}).timeout(10000);
	});

	describe("savepoint", function() {
		it("single", async function() {
			await persistence.savepoint();
			await persistence.consistencyCheck();
		}).timeout(10000);
		it("multiple", async function() {
			const promise = [persistence.savepoint(), persistence.savepoint(), persistence.savepoint()];
			await Promise.all(promise);
			await persistence.consistencyCheck();
		}).timeout(10000);
	});

	describe("reset", function() {
		it("no data", async function() {
			await persistence.reset();
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.deepStrictEqual(data, {}, JSON.stringify(data));
		}).timeout(10000);
		it("data", async function() {
			const d = {
				hello: "world",
				yes: [0, 1, 3]
			};
			await persistence.reset(d);
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.deepStrictEqual(data, d, JSON.stringify(data));
		}).timeout(10000);
	});

	describe("use case 1", function() {
		it("reset", async function() {
			await persistence.reset();
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.deepStrictEqual(data, {});
		}).timeout(10000);
		it("set a -> b", async function() {
			await persistence.write("set", "a", "b");
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.deepStrictEqual(data, { a: "b" });
		}).timeout(10000);
		it("restart", async function() {
			await persistence.reset({ a: "b" });
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.deepStrictEqual(data, { a: "b" });
		}).timeout(10000);
		it("reset", async function() {
			await persistence.reset();
			await persistence.consistencyCheck();
			const data = await persistence.get();
			Assert.deepStrictEqual(data, {});
		}).timeout(10000);
	});

	describe("stress synchronous", function() {
		// Perform many operations asynchronously
		let commandQueue = [];
		let expectedValue = {};
		it("reset", async function() {
			await persistence.reset();
			Assert.deepStrictEqual(await persistence.get(), expectedValue);
		}).timeout(10000);

		let nbOperationsLeft = persistenceOptions.iterations;
		it("run (" + nbOperationsLeft + " iterations)", async function() {
			while (nbOperationsLeft--) {
				const randomValue = Math.floor(Math.random() * Math.floor(5));
				process.stdout.write(randomValue + " ");
				switch (randomValue) {
				case 0:
					{
						const key = Math.random();
						const value = Math.random();

						commandQueue.push("add value " + key + " -> " + value);

						await persistence.write("set", key, value);
						expectedValue[key] = value;
						await persistence.consistencyCheck();
						Assert.ok(persistence.isDirty(), JSON.stringify(commandQueue));
						Assert.deepStrictEqual(await persistence.get(), expectedValue, JSON.stringify(commandQueue));
					}
					break;
				case 1:
					{
						const keyList = Object.keys(expectedValue);
						const key = keyList[Math.floor(Math.random() * keyList.length)];

						commandQueue.push("delete key " + key);

						await persistence.write("delete", key);
						delete expectedValue[key];
						await persistence.consistencyCheck();
						Assert.ok(persistence.isDirty(), JSON.stringify(commandQueue));
						Assert.deepStrictEqual(await persistence.get(), expectedValue, JSON.stringify(commandQueue));
					}
					break;
				case 2:
					{
						commandQueue.push("savepoint");

						await persistence.savepoint();
						await persistence.consistencyCheck();
						Assert.ok(!persistence.isDirty(), JSON.stringify(commandQueue));
						Assert.deepStrictEqual(await persistence.get(), expectedValue, JSON.stringify(commandQueue));
					}
					break;
				case 3:
					{
						commandQueue.push("restart");

						persistence.close();
						persistence = await PersistenceDisk.make(persistenceOptions.path, persistenceOptions);
						await persistence.consistencyCheck();
						Assert.deepStrictEqual(await persistence.get(), expectedValue, JSON.stringify(commandQueue));
					}
					break;
				case 4:
					{
						commandQueue.push("reset");

						await persistence.reset(expectedValue);
						await persistence.consistencyCheck();
						Assert.ok(!persistence.isDirty(), JSON.stringify(commandQueue));
						Assert.deepStrictEqual(await persistence.get(), expectedValue, JSON.stringify(commandQueue));
					}
					break;
				}
			}
		}).timeout(120000);
	});

	describe("stress asynchronous", function() {
		let expectedValue = {};
		let commandQueue = [];

		it("reset", async function() {
			await persistence.reset();
			Assert.deepStrictEqual(await persistence.get(), expectedValue);
		}).timeout(10000);

		let nbOperationsLeft = persistenceOptions.iterations;
		it("run (" + nbOperationsLeft + " iterations)", async function() {
			while (nbOperationsLeft--) {
				const randomValue = Math.floor(Math.random() * Math.floor(4));
				switch (randomValue) {
				case 0:
					{
						const key = Math.random();
						const value = Math.random();
						commandQueue.push("add value " + key + " -> " + value);
						persistence.write("set", key, value);
						expectedValue[key] = value;
					}
					break;
				case 1:
					{
						const keyList = Object.keys(expectedValue);
						const key = keyList[Math.floor(Math.random() * keyList.length)];
						commandQueue.push("delete " + key);

						persistence.write("delete", key);
						delete expectedValue[key];
					}
					break;
				case 2:
					{
						commandQueue.push("savepoint");
						persistence.savepoint();
					}
					break;
				case 3:
					{
						commandQueue.push("reset");
						persistence.reset(expectedValue);
					}
					break;
				}
			}

			await persistence.waitSync();
			await persistence.consistencyCheck();
			Assert.deepStrictEqual(await persistence.get(), expectedValue, JSON.stringify(commandQueue));

			// Close the persistence
			persistence.close();
		}).timeout(120000);
	});
});
