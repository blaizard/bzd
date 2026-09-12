import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import SessionAuthenticationServer from "#bzd/nodejs/core/authentication/session/server.js";
import Session from "#bzd/nodejs/core/authentication/session.js";
import KeyValueStoreMemory from "#bzd/nodejs/db/key_value_store/memory.js";

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

	describe("_verifyAccessToken", () => {
		const inheritedProperties = ["toString", "constructor", "valueOf", "hasOwnProperty", "isPrototypeOf"];

		it("rejects forged access tokens using inherited prototype property names", async () => {
			const server = new SessionAuthenticationServer({
				kvs: new KeyValueStoreMemory("session-authentication-test"),
				kvsBucket: "sessions",
			});
			const uid = "testuid";

			// Create a legitimate session so a real session entry exists in the KVS.
			const session = new Session(uid, ["read"]);
			const maybeAccessToken = await server._makeAccessToken(session);
			Exception.assert(maybeAccessToken !== false, "A legitimate access token must be created.");

			// A genuine token still verifies successfully.
			Exception.assert(
				(await server._verifyAccessToken(maybeAccessToken.token)) !== false,
				"A genuine token must verify.",
			);

			// Forged tokens using inherited property names must be rejected.
			for (const name of inheritedProperties) {
				const forgedToken = server._makeToken(uid, name);
				Exception.assert(
					(await server._verifyAccessToken(forgedToken)) === false,
					"Forged access token '{}' must be rejected.",
					forgedToken,
				);
			}
		});
	});
});
