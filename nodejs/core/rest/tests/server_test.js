import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import MockHttpServer from "#bzd/nodejs/core/http/mock/server.js";
import RestServer from "#bzd/nodejs/core/rest/server.js";

const Exception = ExceptionFactory("test", "rest", "server");

describe("RestServer", () => {
	const makeServer = async () => {
		const mockChannel = new MockHttpServer();
		const rest = new RestServer(
			{
				"/echo": {
					post: {
						request: {
							type: "json",
						},
						response: {
							type: "json",
						},
					},
				},
			},
			{ channel: mockChannel },
		);
		rest.handle("post", "/echo", async (data) => data);
		await mockChannel.start();
		return mockChannel;
	};

	describe("json body", () => {
		it("accepts a valid JSON object body", async () => {
			const mockChannel = await makeServer();
			const response = await mockChannel.send("post", "/api/v1/echo", {
				data: { hello: "world" },
			});
			Exception.assertEqual(response.status, 200);
			Exception.assertEqual(response.data, { hello: "world" });
		});

		it("returns 400 when the body is missing (e.g. non-JSON content-type)", async () => {
			const mockChannel = await makeServer();
			// A text/plain content-type is skipped by the body parsers, leaving the body undefined.
			const response = await mockChannel.send("post", "/api/v1/echo", {});
			Exception.assertEqual(response.status, 400);
		});

		it("returns 400 when the body is not a JSON object", async () => {
			const mockChannel = await makeServer();
			const response = await mockChannel.send("post", "/api/v1/echo", {
				data: "not-an-object",
			});
			Exception.assertEqual(response.status, 400);
		});

		it("returns 200 for a JSON content-type with parameters", async () => {
			const mockChannel = await makeServer();
			const response = await mockChannel.send("post", "/api/v1/echo", {
				headers: { "content-type": "application/json; charset=utf-8" },
				data: { hello: "world" },
			});
			Exception.assertEqual(response.status, 200);
		});

		it("returns 400 when the body is sent with urlencoded content-type", async () => {
			const mockChannel = await makeServer();
			const response = await mockChannel.send("post", "/api/v1/echo", {
				headers: { "content-type": "application/x-www-form-urlencoded" },
				data: { '{"hello":"world"}': "" },
			});
			Exception.assertEqual(response.status, 400);
		});

		it("returns 400 for any content-type that does not comply with JSON", async () => {
			const mockChannel = await makeServer();
			const response = await mockChannel.send("post", "/api/v1/echo", {
				headers: { "content-type": "text/plain" },
				data: { hello: "world" },
			});
			Exception.assertEqual(response.status, 400);
		});
	});
});
