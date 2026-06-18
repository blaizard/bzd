import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { HttpClient } from "#bzd/nodejs/core/http/client.mjs";

const Exception = ExceptionFactory("test", "http");

describe("Node HTTP", () => {
	describe("Simple", () => {
		it("redirection", async () => {
			const stream = await HttpClient.get(
				"https://github.com/bazelbuild/bazel/releases/download/9.1.1/bazel-9.1.1-dist.zip",
				{
					headers: {
						"user-agent": "bzd/proxy 1.0",
						accept: "*/*",
						connection: "keep-alive",
						host: "github.com",
					},
					expect: "stream",
					timeoutMs: 2000,
				},
			);

			let nbBytes = 0;
			for await (const chunk of stream) {
				nbBytes += chunk.length;
			}
			Exception.assertEqual(nbBytes, 221348145);
		}).timeout(100000);
	});
});
