import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.js";

const Exception = ExceptionFactory("test", "token");

describe("TokenInfo", () => {
	describe("make", () => {
		it("stores the class lifetime", () => {
			const token = TokenInfo.make("identifier", ["scope"], 30, /*rolling*/ true);
			Exception.assertEqual(token.data().lifetime, 30);
			Exception.assert(token.isRolling(), "The token must be rolling.");
		});
	});

	describe("updateMinDuration", () => {
		it("extends to the requested minimum duration when below the floor", () => {
			const token = TokenInfo.make("identifier", ["scope"], 1800, /*rolling*/ true);
			token.data().expiration = Date.now() + 1000;
			token.updateMinDuration(300);
			const duration = token.duration();
			Exception.assert(duration >= 300 && duration < 310, "Duration must be at least the floor: {}", duration);
		});

		it("does not shorten the token when it has enough duration left", () => {
			const token = TokenInfo.make("identifier", ["scope"], 1800, /*rolling*/ true);
			token.updateMinDuration(300);
			const duration = token.duration();
			Exception.assert(duration > 300, "Duration must not be shortened: {}", duration);
		});

		it("bounds a huge minimum duration to the class lifetime", () => {
			const token = TokenInfo.make("identifier", ["scope"], 30, /*rolling*/ true);
			token.updateMinDuration(31536000);
			const duration = token.duration();
			Exception.assert(duration <= 31, "Duration must be bounded by the class lifetime: {}", duration);
			Exception.assert(duration >= 29, "Duration must be extended up to the class lifetime: {}", duration);
		});

		it("bounds a huge minimum duration for a legacy token without lifetime", () => {
			const token = new TokenInfo({
				identifier: "identifier",
				scopes: ["scope"],
				creation: Date.now() - 10000,
				expiration: Date.now() + 20000,
				rolling: true,
			});
			token.updateMinDuration(31536000);
			const duration = token.duration();
			Exception.assert(duration <= 31, "Duration must be bounded by the legacy lifetime: {}", duration);
		});

		it("keeps sliding within the class lifetime without a hard deadline", () => {
			const token = TokenInfo.make("identifier", ["scope"], 1800, /*rolling*/ true);
			for (let i = 0; i < 100; i++) {
				token.data().expiration = Date.now() + 1;
				token.updateMinDuration(300);
				const duration = token.duration();
				Exception.assert(duration >= 300, "The sliding window must keep extending: {}", duration);
				Exception.assert(duration <= 1801, "The sliding window must stay within the class lifetime: {}", duration);
			}
		});
	});
});
