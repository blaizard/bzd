import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import SessionAuthenticationServer from "#bzd/nodejs/core/authentication/session/server.js";

const Exception = ExceptionFactory("test", "session");

describe("SessionAuthenticationServer", () => {
	describe("_makeTokenHash", () => {
		it("returns a 64 characters hex string", () => {
			const server = new SessionAuthenticationServer({});
			const hash = server._makeTokenHash();
			Exception.assert(/^[0-9a-f]{64}$/.test(hash), "The token hash must be a 64 characters hex string, got: {}", hash);
		});

		it("returns distinct values across successive calls", () => {
			const server = new SessionAuthenticationServer({});
			const hashes = new Set();
			for (let i = 0; i < 100; i++) {
				hashes.add(server._makeTokenHash());
			}
			Exception.assertEqual(hashes.size, 100);
		});
	});
});
