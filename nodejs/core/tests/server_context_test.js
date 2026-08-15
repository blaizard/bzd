import ExceptionFactory from "../exception.js";
import MockServerContext from "#bzd/nodejs/core/http/mock/server_context.js";

const Exception = ExceptionFactory("test", "serverContext");

describe("HttpServerContextCommon", () => {
	describe("getIp", () => {
		const makeContext = (trustedProxies, request) => {
			if (trustedProxies) {
				process.env.BZD_TRUSTED_PROXIES = trustedProxies;
			} else {
				delete process.env.BZD_TRUSTED_PROXIES;
			}
			return MockServerContext.make(request);
		};

		it("returns the peer IP when no header is present", () => {
			const context = makeContext(null, { socket: { remoteAddress: "10.0.0.1" } });
			Exception.assertEqual(context.getIp(), "10.0.0.1");
		});

		it("ignores the header when the peer is not a trusted proxy", () => {
			const context = makeContext(null, {
				socket: { remoteAddress: "10.0.0.1" },
				headers: { "x-forwarded-for": "1.2.3.4" },
			});
			Exception.assertEqual(context.getIp(), "10.0.0.1");
		});

		it("returns the rightmost non-trusted entry when the peer is trusted", () => {
			const context = makeContext("10.0.0.1", {
				socket: { remoteAddress: "10.0.0.1" },
				headers: { "x-forwarded-for": "1.2.3.4, 10.0.0.1" },
			});
			Exception.assertEqual(context.getIp(), "1.2.3.4");
		});

		it("walks a multi-hop chain of trusted proxies", () => {
			const context = makeContext("10.0.0.1, 10.0.0.2", {
				socket: { remoteAddress: "10.0.0.1" },
				headers: { "x-forwarded-for": "1.2.3.4, 10.0.0.2, 10.0.0.1" },
			});
			Exception.assertEqual(context.getIp(), "1.2.3.4");
		});

		it("falls back to the peer IP when all header entries are trusted proxies", () => {
			const context = makeContext("10.0.0.1, 10.0.0.2", {
				socket: { remoteAddress: "10.0.0.1" },
				headers: { "x-forwarded-for": "10.0.0.2, 10.0.0.1" },
			});
			Exception.assertEqual(context.getIp(), "10.0.0.1");
		});

		it("falls back to the peer IP when the header is absent but the peer is trusted", () => {
			const context = makeContext("10.0.0.1", { socket: { remoteAddress: "10.0.0.1" } });
			Exception.assertEqual(context.getIp(), "10.0.0.1");
		});

		it("normalizes the IPv4-mapped IPv6 prefix", () => {
			const context = makeContext(null, { socket: { remoteAddress: "::ffff:10.0.0.1" } });
			Exception.assertEqual(context.getIp(), "10.0.0.1");
		});

		it("normalizes the IPv4-mapped prefix in the trusted proxies and header entries", () => {
			const context = makeContext("::ffff:10.0.0.1", {
				socket: { remoteAddress: "::ffff:10.0.0.1" },
				headers: { "x-forwarded-for": "::ffff:1.2.3.4, ::ffff:10.0.0.1" },
			});
			Exception.assertEqual(context.getIp(), "1.2.3.4");
		});

		it("returns null when there is no socket peer", () => {
			const context = makeContext(null, { headers: { "x-forwarded-for": "1.2.3.4" } });
			Exception.assertEqual(context.getIp(), null);
		});
	});
});
