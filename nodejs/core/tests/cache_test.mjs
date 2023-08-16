import Cache from "../cache.mjs";
import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("test", "cache");

describe("Cache", () => {
	describe("Simple", () => {
		it("Base", async () => {
			let cache = new Cache({
				garbageCollector: false,
			});
			let argRead = 0;
			cache.register("test", (arg) => {
				if (!arg) {
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
	});

	it("Stress", async () => {
		let cache = new Cache({
			garbageCollector: false,
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
				timeout: 10,
			}
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
				})()
			);
		}

		await Promise.all(promises);

		Exception.assertEqual(sum, expected);
	}).timeout(10000);
});
