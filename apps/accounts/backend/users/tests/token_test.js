import ClockMock from "#bzd/nodejs/core/clock/mock.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.js";

const Exception = ExceptionFactory("test", "token");

describe("TokenInfo", () => {
	let clock;

	beforeEach(() => {
		clock = new ClockMock();
	});

	describe("make", () => {
		it("stores the class lifetime", () => {
			const token = TokenInfo.make("identifier", ["scope"], 30, /*rolling*/ true, clock);
			Exception.assertEqual(token.data().lifetime, 30);
			Exception.assert(token.isRolling(), "The token must be rolling.");
		});
	});

	describe("updateMinDuration", () => {
		it("extends to the requested minimum duration when below the floor", () => {
			const token = TokenInfo.make("identifier", ["scope"], 1800, /*rolling*/ true, clock);
			token.data().expiration = clock.getTimeMs() + 1000;
			token.updateMinDuration(300);
			Exception.assertEqual(token.duration(), 300);
		});

		it("does not shorten the token when it has enough duration left", () => {
			const token = TokenInfo.make("identifier", ["scope"], 1800, /*rolling*/ true, clock);
			const expiration = token.data().expiration;
			token.updateMinDuration(300);
			Exception.assertEqual(token.data().expiration, expiration);
			Exception.assertEqual(token.duration(), 1800);
		});

		it("bounds a huge minimum duration to the class lifetime", () => {
			const token = TokenInfo.make("identifier", ["scope"], 30, /*rolling*/ true, clock);
			token.updateMinDuration(31536000);
			Exception.assertEqual(token.duration(), 30);
		});

		it("bounds a huge minimum duration for a legacy token without lifetime", () => {
			const nowMs = clock.getTimeMs();
			const token = new TokenInfo(
				{
					identifier: "identifier",
					scopes: ["scope"],
					creation: nowMs - 10000,
					expiration: nowMs + 20000,
					rolling: true,
				},
				clock,
			);
			token.updateMinDuration(31536000);
			Exception.assertEqual(token.duration(), 30);
		});

		it("keeps sliding within the class lifetime without a hard deadline", () => {
			const token = TokenInfo.make("identifier", ["scope"], 1800, /*rolling*/ true, clock);
			for (let i = 0; i < 100; i++) {
				clock.advanceMs(1000);
				token.data().expiration = clock.getTimeMs() + 1;
				token.updateMinDuration(300);
				Exception.assertEqual(token.duration(), 300);
			}
		});
	});
});
