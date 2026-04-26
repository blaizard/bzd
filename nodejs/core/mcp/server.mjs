import { ExceptionFactory } from "../exception.mjs";
import LogFactory from "../log.mjs";
import { isObject } from "#bzd/nodejs/utils/object.mjs";

const Exception = ExceptionFactory("mcp", "server");
const Log = LogFactory("mcp", "server");

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
	/// Only 'doc' fields can be updated - all other fields from schema are preserved.
	/// @param schema The existing schema to update
	/// @param content The new documentation to merge (only 'doc' fields will be used)
	/// @returns A new schema with updated documentation
	/// @throws Error if content contains keys that don't exist in schema (except 'doc')
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
			if (key === "doc") {
				result[key] = content[key];
			} else if (isObject(content[key])) {
				Exception.assert(isObject(result[key]), "Invalid key '{}' in content - only 'doc' fields can be updated", key);
				result[key] = this.updateSchema(result[key], content[key]);
			} else {
				Exception.error("Invalid key '{}' in content - only 'doc' fields can be updated", key);
			}
		}

		return result;
	}

	/// Assert that the schema provided is correct.
	static assertSchema(schema) {
		const supportedParameterTypes = ["string"];
		Exception.assert(isObject(schema), "The schema must be an object: {:j}", schema);
		for (const [name, tool] of Object.entries(schema.tools || {})) {
			Exception.assert("doc" in tool, "The tool '{}' is missing a documentation.", name);
			for (const [parameterName, parameter] of Object.entries(tool.parameters || {})) {
				Exception.assert(
					"doc" in parameter,
					"The parameter '{}' in tool '{}' is missing a documentation.",
					parameterName,
					name,
				);
				Exception.assert(
					"type" in parameter,
					"The parameter '{}' in tool '{}' is missing a type.",
					parameterName,
					name,
				);
				Exception.assert(
					supportedParameterTypes.includes(parameter.type),
					"The parameter '{}' in tool '{}' should be one of {:j}, not '{}'.",
					parameterName,
					name,
					supportedParameterTypes,
					parameter.type,
				);
			}
		}
	}

	/// Register a mcp endpoint
	/// 	handle("artifacts.plugins.nodes", "@mcp", callback) {
	/// Support SSE endpoint definition.
	addRoute(endpoint, callback, schema, options) {
		Exception.assert(this.options.channel, "Channel is missing");
		MCPServer.assertSchema(schema);

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
							description: tool.doc,
							inputSchema: {
								type: "object",
								properties: Object.fromEntries(
									Object.entries(tool.parameters || {}).map(([parameterName, parameter]) => {
										return [
											parameterName,
											{
												description: parameter.doc,
												type: parameter.type,
											},
										];
									}),
								),
							},
						};
					});
					response.result = {
						tools: tools,
					};
				} else if (method === "tools/call") {
					const body = context.getBody() || {};
					const tool = body.params?.name;
					const args = body.params?.arguments;
					Exception.assertPrecondition(tool && args, "Missing tool and arguments: {:j}", body);
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
									text: e.toString(),
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

				context.setHeader("Content-Type", "application/json");
				context.sendStatus(200, JSON.stringify(response));
			},
			{
				type: "json",
			},
		);
	}
}
