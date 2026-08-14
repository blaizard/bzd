import ExceptionFactory from "../exception.js";
import RateLimiter from "../rate_limit.js";
import Cache2 from "#bzd/nodejs/core/cache2.js";
import ClockMock from "#bzd/nodejs/core/clock/mock.js";

const Exception = ExceptionFactory("test", "rateLimit");

describe("RateLimiter", () => {
	const bucket = "rl";
	const threshold = 3;
	const windowMs = 60_000;

	let cache;
	let clock;
	let limiter;

	beforeEach(() => {
		cache = new Cache2("test-rate");
		clock = new ClockMock();
		limiter = new RateLimiter(cache, {
			bucket,
			threshold,
			windowMs,
			clock,
		});
	});

	/// Record requests until the key reaches the threshold.
	const reachLimit = async (key) => {
		for (let i = 0; i < threshold; ++i) {
			await limiter.record(key);
		}
	};

	describe("record", () => {
		it("crosses the threshold", async () => {
			Exception.assert(await limiter.record("keyA"));
			Exception.assert(await limiter.record("keyA"));
			Exception.assert(!(await limiter.record("keyA")));
			Exception.assert(await limiter.isOverLimit("keyA"));
		});

		it("isolates keys", async () => {
			await reachLimit("keyA");
			Exception.assert(await limiter.isOverLimit("keyA"));
			Exception.assert(await limiter.record("keyB"));
			Exception.assert(!(await limiter.isOverLimit("keyB")));
		});

		it("resets a key", async () => {
			await reachLimit("keyA");
			Exception.assert(await limiter.isOverLimit("keyA"));
			await limiter.reset("keyA");
			Exception.assert(!(await limiter.isOverLimit("keyA")));
			Exception.assert(await limiter.record("keyA"));
		});

		it("expires after the window", async () => {
			await reachLimit("keyA");
			Exception.assert(await limiter.isOverLimit("keyA"));
			clock.advanceMs(windowMs + 1);
			Exception.assert(!(await limiter.isOverLimit("keyA")));
			Exception.assert(await limiter.record("keyA"));
		});

		it("caps stored timestamps at the threshold", async () => {
			for (let i = 0; i < 10; ++i) {
				await limiter.record("keyA");
			}
			const entry = cache.getInstant(bucket, "keyA", null);
			Exception.assert(entry !== null);
			Exception.assertEqual(entry.timestamps.length, threshold);
		});
	});
});
