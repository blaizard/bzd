import MCPServer from "#bzd/nodejs/core/mcp/server.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("test", "mcp", "server");

describe("MCPServer", () => {
	describe("updateSchema", () => {
		it("should return schema when content is empty object", () => {
			const schema = { doc: "old" };
			const result = MCPServer.updateSchema(schema, {});
			Exception.assertEqual(result, { doc: "old" });
		});

		it("should throw when doc from content is misplaced", async () => {
			const schema = {};
			const content = { doc: "new" };
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should update the doc", async () => {
			const schema = { doc: "old" };
			const content = { doc: "new" };
			const result = MCPServer.updateSchema(schema, content);
			Exception.assertEqual(result, { doc: "new" });
		});

		it("should use the same type", async () => {
			const schema = { doc: "old" };
			const content = { doc: true };
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should update nested doc", async () => {
			const schema = {
				doc: "old",
				another: {
					doc: "nested old",
				},
			};
			const content = {
				doc: "new",
				another: {
					doc: "nested new",
				},
			};
			const result = MCPServer.updateSchema(schema, content);
			Exception.assertEqual(result, {
				doc: "new",
				another: {
					doc: "nested new",
				},
			});
		});

		it("should handle deeply nested invalid key", async () => {
			const schema = {
				tool1: {
					properties: {
						param1: {
							doc: "parameter 1",
						},
					},
				},
			};
			const content = {
				tool1: {
					properties: {
						param1: {
							invalidKey: "value",
						},
					},
				},
			};
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should throw when content is array", async () => {
			const schema = { doc: "test" };
			const content = [];
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should throw when content is number", async () => {
			const schema = { doc: "test" };
			const content = 123;
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should throw when content has non-doc primitive at object key", async () => {
			const schema = { doc: "test" };
			const content = { tool1: "string" };
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should handle mixed valid doc and invalid keys", async () => {
			const schema = {
				tool1: {
					doc: "old",
					properties: {
						param1: { doc: "param" },
					},
				},
			};
			const content = {
				tool1: {
					doc: "new",
					invalidKey: "value",
				},
			};
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});
	});
});
