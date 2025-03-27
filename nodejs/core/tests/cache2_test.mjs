import Cache2 from "../cache2.mjs";
import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("test", "cache2");

describe("Cache2", () => {
	describe("Simple", () => {
		it("Base", async () => {
			let cache = new Cache2();
			let argRead = 0;
			cache.register("test", (arg) => {
				if (arg === false) {
					throw new Error("BOOM!");
				}
				argRead = arg;
				return "return." + arg;
			});
			const result1 = await cache.get("test", "hello");
			Exception.assertEqual(argRead, "hello");
			Exception.assertEqual(result1, "return.hello");
			const result2 = await cache.get("test", "world");
			Exception.assertEqual(argRead, "world");
			Exception.assertEqual(result2, "return.world");
			argRead = "no";
			const result3 = await cache.get("test", "world");
			Exception.assertEqual(argRead, "no");
			Exception.assertEqual(result3, "return.world");

			await Exception.assertThrows(async () => {
				await cache.get("tefst", "world");
			});
			await Exception.assertThrows(async () => {
				await cache.get("test", false);
			});
			await Exception.assertThrows(async () => {
				await cache.get("tedst");
			});
			await Exception.assertThrows(async () => {
				await cache.get();
			});
		});

		it("Array Of String", async () => {
			let cache = new Cache2();
			let value = 0;
			cache.register("test", (arg) => {
				return value++;
			});

			{
				const result = await cache.get("test", Cache2.arrayOfStringToKey([]));
				Exception.assertEqual(result, 0);
			}
			{
				const result = await cache.get("test", Cache2.arrayOfStringToKey([""]));
				Exception.assertEqual(result, 1);
			}
			{
				const result = await cache.get("test", Cache2.arrayOfStringToKey(["", ""]));
				Exception.assertEqual(result, 2);
			}
			{
				const result = await cache.get("test", Cache2.arrayOfStringToKey([]));
				Exception.assertEqual(result, 0);
			}
		});
	});

	it("Concurrency", async () => {
		let cache = new Cache2({
			timeoutMs: 20 * 1000,
		});
		let value = 1;
		cache.register(
			"test",
			(key) => {
				// This fetcher should run only once.
				return new Promise((resolve) => {
					setTimeout(() => {
						resolve(value++);
					}, 10);
				});
			},
			{
				timeoutMs: 60 * 1000,
			},
		);

		let sum = 0;
		let promises = [];
		for (let i = 0; i < 10; ++i) {
			promises.push(
				(async () => {
					const result = await cache.get("test", "hello");
					sum += result;
				})(),
			);
		}
		await Promise.all(promises);
		Exception.assertEqual(sum, 10);
	});

	it("Order", async () => {
		const cache = new Cache2();
		cache.register(
			"test",
			(key) => key,
		);
		const getKeys = () => [...cache.data["test"].values.keys()];
		Exception.assertEqual(getKeys(), []);
		await cache.get("test", "a");
		Exception.assertEqual(getKeys(), ["a"]);
		await cache.get("test", "b");
		await cache.get("test", "c");
		Exception.assertEqual(getKeys(), ["a", "b", "c"]);
		await cache.get("test", "b");
		Exception.assertEqual(getKeys(), ["a", "c", "b"]);
		cache.set("test", "c", 1);
		Exception.assertEqual(getKeys(), ["a", "b", "c"]);
		cache.setDirty("test", "a");
		Exception.assertEqual(getKeys(), ["a", "b", "c"]);
	});

	it("Stress", async () => {
		let cache = new Cache2({
			timeoutMs: 20 * 1000,
		});

		let sum = 0;
		let expected = 0;
		cache.register(
			"test",
			(arg) => {
				return new Promise((resolve) => {
					setTimeout(() => {
						resolve(parseInt(arg));
					}, Math.random() * 10);
				});
			},
			{
				timeoutMs: 10,
			},
		);

		const promiseDelay = (t) => {
			return new Promise((resolve) => {
				setTimeout(resolve, t);
			});
		};

		let promises = [];
		// eslint-disable-next-line
		for (const i in [...Array(10000).keys()]) {
			const n = Math.floor(Math.random() * 10);
			expected += parseInt(n);
			promises.push(
				(async () => {
					await promiseDelay(Math.random() * 10);
					const result = await cache.get("test", n);
					sum += result;
				})(),
			);
		}

		await Promise.all(promises);

		Exception.assertEqual(sum, expected);
	}).timeout(10000);
});
