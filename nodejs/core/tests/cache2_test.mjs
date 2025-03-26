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
