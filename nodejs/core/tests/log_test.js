import LogFactory from "#bzd/nodejs/core/log.js";
import Cache from "#bzd/nodejs/core/cache.js";
import ClockMock from "#bzd/nodejs/core/clock/mock.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("test", "log");

const Log = LogFactory("test", "rateLimit");

describe("Logger", () => {
	describe("rate limiting", () => {
		let clock;
		let messages;

		const capture = (_date, _level, _topics, message) => {
			messages.push(message);
		};

		before(() => {
			Log.config.console.mute();
			clock = new ClockMock();
			Log.setRateLimiter(new Cache("test-log"), { clock: clock });
		});

		beforeEach(() => {
			messages = [];
			Log.addBackend("capture", capture, "trace");
		});

		it("prints up to the threshold of identical logs within the window", () => {
			Log.info("burst {}", "a");
			Log.info("burst {}", "a");
			Log.info("burst {}", "a");
			Exception.assertEqual(messages.length, 2);
		});

		it("treats logs with different level, topics, or str as distinct", () => {
			Log.info("distinct {}", "a");
			Log.warning("distinct {}", "a");
			Log.info("other {}", "a");
			Exception.assertEqual(messages.length, 3);
		});

		it("treats logs with the same format but different values as distinct", () => {
			Log.info("message {}", 1);
			Log.info("message {}", 2);
			Log.info("message {}", 3);
			Exception.assertEqual(messages.length, 3);
		});

		it("reports the suppressed logs once the window is over, then resets", () => {
			Log.info("window {}", "a");
			Log.info("window {}", "a");
			Log.info("window {}", "a");
			Exception.assertEqual(messages.length, 2);
			clock.advanceMs(5000);
			Log.info("window {}", "a");
			Exception.assertEqual(messages.length, 3);
			Exception.assertEqual(messages[2], "[skipped 1 logs] window a");
			Log.info("window {}", "a");
			Exception.assertEqual(messages[3], "window a");
		});
	});
});
