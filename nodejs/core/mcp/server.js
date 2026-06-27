import { ExceptionFactory } from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import ValidationSchema from "#bzd/nodejs/core/validation_schema.js";
import { isObject } from "#bzd/nodejs/utils/object.js";

const Exception = ExceptionFactory("mcp", "server");
const Log = LogFactory("mcp", "server");

const mcpSchemaDescriptor = {
	type: "object",
	properties: {
		tools: {
			type: "object",
			additionalProperties: {
				type: "object",
				properties: {
					description: {
						type: "string",
					},
					parameters: {
						type: "object",
						additionalProperties: true,
					},
				},
				required: ["description"],
			},
		},
	},
};

export default class MCPServer {
	constructor(options) {
		this.options = Object.assign(
			{
				/// Version of the API to be used.
				version: 1,
				/// Channel to be used as a transportation mean for this REST API
				channel: null,
			},
			options,
		);
		this.sessions = {};
	}

	/// Update a schema with new documentation.
	/// Only 'description' fields can be updated - all other fields from schema are preserved.
	/// @param schema The existing schema to update
	/// @param content The new documentation to merge (only 'description' fields will be used)
	/// @returns A new schema with updated documentation
	/// @throws Error if content contains keys that don't exist in schema (except 'description')
	static updateSchema(schema, content) {
		Exception.assert(isObject(content), "Schema override must be an object");

		// Clone schema to avoid mutation
		const result = { ...schema };

		// Validate and process each key in content
		for (const key of Object.keys(content)) {
			Exception.assert(key in result, "Invalid key '{}' in schema override, not existing in original schema", key);
			Exception.assert(
				typeof result[key] === typeof content[key],
				"The value type for key '{}' differ from the schema: {} vs {}",
				key,
				typeof result[key],
				typeof content[key],
			);
			if (key === "description") {
				result[key] = content[key];
			} else if (isObject(content[key])) {
				Exception.assert(
					isObject(result[key]),
					"Invalid key '{}' in content - only 'description' fields can be updated",
					key,
				);
				result[key] = this.updateSchema(result[key], content[key]);
			} else {
				Exception.error("Invalid key '{}' in content - only 'description' fields can be updated", key);
			}
		}

		return result;
	}

	/// Register a mcp endpoint
	/// 	handle("artifacts.plugins.nodes", "@mcp", callback) {
	/// Support SSE endpoint definition.
	addRoute(endpoint, callback, schema, options) {
		Exception.assert(this.options.channel, "Channel is missing");
		new ValidationSchema(mcpSchemaDescriptor).validate(schema);

		const parametersValidationSchema = Object.fromEntries(
			Object.entries(schema.tools || {}).map(([name, data]) => {
				return [name, new ValidationSchema(data.parameters || {})];
			}),
		);

		// Enable CORS Headers so that it can be reached from browsers.
		this.options.channel.addRoute("OPTIONS", endpoint, (context) => {
			context.setHeader("Access-Control-Allow-Origin", "*");
			context.setHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
			context.setHeader(
				"Access-Control-Allow-Headers",
				"Origin, Content-Type, Accept, Authorization, mcp-protocol-version",
			);
			context.sendStatus(200, "OK");
		});

		this.options.channel.addRoute(
			"POST",
			endpoint,
			async (context) => {
				const body = context.getBody();
				const method = body.method;
				let response = {
					jsonrpc: "2.0",
					id: body.id,
				};
				if (method === "initialize") {
					let capabilities = {};
					if (schema.tools) {
						capabilities["tools"] = {};
					}
					response.result = {
						protocolVersion: "2024-11-05",
						capabilities: capabilities,
						serverInfo: { name: "bzd-mcp", version: "1.0.0" },
					};
				} else if (method === "tools/list") {
					const tools = Object.entries(schema.tools || {}).map(([name, tool]) => {
						return {
							name: name,
							description: tool.description,
							inputSchema: tool.parameters || { type: "object", properties: {} },
						};
					});
					response.result = {
						tools: tools,
					};
				} else if (method === "tools/call") {
					const body = context.getBody() || {};
					const tool = body.params?.name;
					const args = body.params?.arguments ?? {};
					Exception.assertPrecondition(tool && args, "Missing tool and arguments: {:?}", body);
					Exception.assertPrecondition(typeof tool === "string", "Tool must be a string, not: {:?}", tool);
					Exception.assertPrecondition(isObject(args), "Arguments must be a dictionary, not: {:?}", args);
					Exception.assertPrecondition(tool in schema.tools, "Unknown tool '{}'", tool);
					Exception.assert(
						tool in parametersValidationSchema,
						"Parameter validation should be available for tool '{}'",
						tool,
					);
					try {
						parametersValidationSchema[tool].validate(args);
					} catch (e) {
						Exception.errorPrecondition("parameters for tool '{}' are invalid: {}", tool, e.message);
					}
					try {
						const data = await callback(tool, args);
						response.result = {
							content: [
								{
									type: "text",
									text: JSON.stringify(data),
								},
							],
							isError: false,
						};
					} catch (e) {
						response.result = {
							content: [
								{
									type: "text",
									text: e.message.toString(),
								},
							],
							isError: true,
						};
					}
				} else {
					response.error = {
						message: "Unsupported MCP method: " + method,
					};
				}

				context.setHeader("Access-Control-Allow-Origin", "*");
				context.setHeader("Access-Control-Expose-Headers", "*");
				context.setHeader("Content-Type", "application/json");
				context.sendStatus(200, JSON.stringify(response));
			},
			{
				type: "json",
			},
		);
	}
}
