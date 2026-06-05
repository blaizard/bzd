import MCPServer from "#bzd/nodejs/core/mcp/server.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import MockHttpServer from "#bzd/nodejs/core/http/mock/server.mjs";

const Exception = ExceptionFactory("test", "mcp", "server");

describe("MCPServer", () => {
	describe("updateSchema", () => {
		it("should return schema when content is empty object", () => {
			const schema = { description: "old" };
			const result = MCPServer.updateSchema(schema, {});
			Exception.assertEqual(result, { description: "old" });
		});

		it("should throw when description from content is misplaced", async () => {
			const schema = {};
			const content = { description: "new" };
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should update the description", async () => {
			const schema = { description: "old" };
			const content = { description: "new" };
			const result = MCPServer.updateSchema(schema, content);
			Exception.assertEqual(result, { description: "new" });
		});

		it("should use the same type", async () => {
			const schema = { description: "old" };
			const content = { description: true };
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should update nested description", async () => {
			const schema = {
				description: "old",
				another: {
					description: "nested old",
				},
			};
			const content = {
				description: "new",
				another: {
					description: "nested new",
				},
			};
			const result = MCPServer.updateSchema(schema, content);
			Exception.assertEqual(result, {
				description: "new",
				another: {
					description: "nested new",
				},
			});
		});

		it("should handle deeply nested invalid key", async () => {
			const schema = {
				tool1: {
					properties: {
						param1: {
							description: "parameter 1",
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
			const schema = { description: "test" };
			const content = [];
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should throw when content is number", async () => {
			const schema = { description: "test" };
			const content = 123;
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should throw when content has non-description primitive at object key", async () => {
			const schema = { description: "test" };
			const content = { tool1: "string" };
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});

		it("should handle mixed valid description and invalid keys", async () => {
			const schema = {
				tool1: {
					description: "old",
					properties: {
						param1: { description: "param" },
					},
				},
			};
			const content = {
				tool1: {
					description: "new",
					invalidKey: "value",
				},
			};
			await Exception.assertThrows(async () => {
				MCPServer.updateSchema(schema, content);
			});
		});
	});

	describe("addRoute", () => {
		it("should throw when channel is null", async () => {
			const mcpServer = new MCPServer({ channel: null });
			await Exception.assertThrowsWithMatch(async () => {
				mcpServer.addRoute("/mcp", async () => {}, { tools: {} });
			}, "Channel is missing");
		});

		it("should throw on invalid schema without description", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			await Exception.assertThrows(async () => {
				mcpServer.addRoute("/mcp", async () => {}, {
					tools: {
						badTool: {
							parameters: { type: "object", properties: {} },
						},
					},
				});
			});
		});

		it("should initialize with tools in capabilities", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "A tool",
						parameters: { type: "object", properties: {} },
					},
				},
			};
			const callback = async () => "result";
			mcpServer.addRoute("/mcp", callback, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: { jsonrpc: "2.0", id: 1, method: "initialize" },
			});
			Exception.assertEqual(response.status, 200);
			const result = JSON.parse(response.data).result;
			Exception.assertEqual(result.protocolVersion, "2024-11-05");
			Exception.assertEqual(result.capabilities.tools, {});
			Exception.assertEqual(result.serverInfo.name, "bzd-mcp");
			Exception.assertEqual(result.serverInfo.version, "1.0.0");
		});

		it("should initialize with empty capabilities when no tools", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {};
			const callback = async () => "result";
			mcpServer.addRoute("/mcp", callback, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: { jsonrpc: "2.0", id: 1, method: "initialize" },
			});
			const body = JSON.parse(response.data);
			Exception.assert(!("tools" in body.result.capabilities));
		});

		it("should return tool metadata for tools/list", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: {
							type: "object",
							properties: { x: { type: "number" } },
						},
					},
				},
			};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: { jsonrpc: "2.0", id: 2, method: "tools/list" },
			});
			const result = JSON.parse(response.data).result;
			Exception.assertEqual(result.tools.length, 1);
			Exception.assertEqual(result.tools[0].name, "tool1");
			Exception.assertEqual(result.tools[0].description, "Tool one");
			Exception.assertEqual(result.tools[0].inputSchema, schema.tools.tool1.parameters);
		});

		it("should return empty tools array for tools/list when no tools", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: { jsonrpc: "2.0", id: 3, method: "tools/list" },
			});
			const result = JSON.parse(response.data).result;
			Exception.assertEqual(result.tools, []);
		});

		it("should invoke callback and return success for tools/call", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: { type: "object", properties: {} },
					},
				},
			};
			const callback = async (name, args) => {
				return { value: 42 };
			};
			mcpServer.addRoute("/mcp", callback, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: {
					jsonrpc: "2.0",
					id: 4,
					method: "tools/call",
					params: { name: "tool1", arguments: {} },
				},
			});
			const result = JSON.parse(response.data).result;
			Exception.assertEqual(result.isError, false);
			Exception.assertEqual(result.content[0].type, "text");
			Exception.assertEqual(result.content[0].text, JSON.stringify({ value: 42 }));
		});

		it("should return error when callback throws for tools/call", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: { type: "object", properties: {} },
					},
				},
			};
			mcpServer.addRoute(
				"/mcp",
				async () => {
					throw new Error("execution failed");
				},
				schema,
			);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: {
					jsonrpc: "2.0",
					id: 4,
					method: "tools/call",
					params: { name: "tool1", arguments: {} },
				},
			});
			const result = JSON.parse(response.data).result;
			Exception.assertEqual(result.isError, true);
			Exception.assertEqual(result.content[0].text, "execution failed");
		});

		it("should return 400 for unknown tool in tools/call", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: { type: "object", properties: {} },
					},
				},
			};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: {
					jsonrpc: "2.0",
					id: 4,
					method: "tools/call",
					params: { name: "unknownTool", arguments: {} },
				},
			});
			Exception.assertEqual(response.status, 400);
			Exception.assert(response.data.includes("Unknown tool 'unknownTool'"));
		});

		it("should return 400 for invalid arguments in tools/call", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: {
							type: "object",
							properties: { x: { type: "number" } },
							required: ["x"],
						},
					},
				},
			};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: {
					jsonrpc: "2.0",
					id: 4,
					method: "tools/call",
					params: { name: "tool1", arguments: {} },
				},
			});
			Exception.assertEqual(response.status, 400);
			Exception.assert(response.data.includes("parameters for tool 'tool1' are invalid"));
		});

		it("should return 400 when params is missing", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: { type: "object", properties: {} },
					},
				},
			};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: { jsonrpc: "2.0", id: 4, method: "tools/call" },
			});
			Exception.assertEqual(response.status, 400);
			Exception.assert(response.data.includes("Missing tool and arguments"));
		});

		it("should return 400 when params.name is missing", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {
				tools: {
					tool1: {
						description: "Tool one",
						parameters: { type: "object", properties: {} },
					},
				},
			};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: {
					jsonrpc: "2.0",
					id: 4,
					method: "tools/call",
					params: { arguments: {} },
				},
			});
			Exception.assertEqual(response.status, 400);
			Exception.assert(response.data.includes("Missing tool and arguments"));
		});

		it("should return JSON-RPC error for unsupported method", async () => {
			const mockChannel = new MockHttpServer();
			const mcpServer = new MCPServer({ channel: mockChannel });
			const schema = {};
			mcpServer.addRoute("/mcp", async () => {}, schema);
			await mockChannel.start();
			const response = await mockChannel.send("post", "/mcp", {
				data: { jsonrpc: "2.0", id: 5, method: "foo/bar" },
			});
			Exception.assertEqual(response.status, 200);
			const body = JSON.parse(response.data);
			Exception.assertEqual(body.error.message, "Unsupported MCP method: foo/bar");
			Exception.assert(!("result" in body));
		});
	});
});
