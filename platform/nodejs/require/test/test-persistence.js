"use strict";

const Path = require('path');
const Assert = require('assert');
const PersistenceDisk = require('../persistence/disk.js');

const persistenceOptions = {
	path: Path.resolve(__dirname, "test-persistence", "test.persistence.json"),
	savepointOnClose: false,
	iterations: 1000
};

let persistence = new PersistenceDisk(persistenceOptions.path, persistenceOptions);

	describe('Persistence', function() {
		describe('actions', function() {
			it('set hello', async function() {
				await persistence.waitReady();
				await persistence.write("set", "hello", "world");
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.ok(data.hello == "world", persistence.data);
			});
			it('set mister', async function() {
				await persistence.write("set", "mister", ["a"]);
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.ok(data.mister instanceof Array, data);
				Assert.ok(data.mister[0] == "a", data);
			});
			it('delete', async function() {
				await persistence.write("delete", "hello");
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.ok(data.hello == undefined, data);
			});
		});

		describe('savepoint', function() {
			it('single', async function() {
				await persistence.savepoint();
				await persistence.consistencyCheck();
			});
			it('multiple', async function() {
				const promise = [persistence.savepoint(), persistence.savepoint(), persistence.savepoint()];
				await Promise.all(promise);
				await persistence.consistencyCheck();
			});
		});

		describe('reset', function() {
			it('no data', async function() {
				await persistence.reset();
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.deepStrictEqual(data, {}, JSON.stringify(data));
			});
			it('data', async function() {
				const d = {
					hello: "world",
					yes: [0, 1, 3]
				};
				await persistence.reset(d);
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.deepStrictEqual(data, d, JSON.stringify(data));
			});
		});

		describe('use case 1', function() {
			it('reset', async function() {
				await persistence.reset();
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.deepStrictEqual(data, {});
			});
			it('set a -> b', async function() {
				await persistence.write("set", "a", "b");
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.deepStrictEqual(data, {a: "b"});
			});
			it('restart', async function() {
				await persistence.reset({a: "b"});
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.deepStrictEqual(data, {a: "b"});
			});
			it('reset', async function() {
				await persistence.reset();
				await persistence.consistencyCheck();
				const data = await persistence.get();
				Assert.deepStrictEqual(data, {});
			});
		});

		describe('stress synchronous', function() {
			// Perform many operations asynchronously
			let commandQueue = [];
			let expectedValue = {};
			it('reset', async function() {
				await persistence.reset();
				Assert.deepStrictEqual(await persistence.get(), expectedValue);
			});

			let nbOperationsLeft = persistenceOptions.iterations;
			it("run (" + nbOperationsLeft + " iterations)", async function() {
				while (nbOperationsLeft--) {
					const randomValue = Math.floor(Math.random() * Math.floor(5));
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
							persistence = new PersistenceDisk(persistenceOptions.path, persistenceOptions);
							await persistence.waitReady();
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

			}).timeout(10000);
		});

		describe('stress asynchronous', function() {

			let expectedValue = {};
			let commandQueue = [];

			it('reset', async function() {
				await persistence.reset();
				Assert.deepStrictEqual(await persistence.get(), expectedValue);
			});

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

			}).timeout(10000);
		});
	});

