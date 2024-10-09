import StorageBzd from "#bzd/apps/artifacts/plugins/nodes/storage.mjs";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";

function rawBodyParse(body, headersFunc, forceContentType = null, forceCharset = null) {
	const contentTypeHeader = Object.fromEntries(
		("type=" + headersFunc("content-type"))
			.split(";")
			.map((s) => s.trim().toLowerCase().split("=", 2))
			.filter(Boolean),
	);

	// Identify the contentType
	let contentType = null;
	if (forceContentType || "type" in contentTypeHeader) {
		switch (forceContentType || contentTypeHeader.type) {
			case "application/json":
				contentType = "json";
				break;
			case "text/css":
			case "text/csv":
			case "text/html":
			case "text/javascript":
			case "text/plain":
			case "text/xml":
				contentType = "text";
				break;
			default:
			// Unsupported!
		}
	}

	// Identify the charset
	let charset = "latin1";
	if (forceCharset || "charset" in contentTypeHeader) {
		// 'ascii', 'utf8', 'utf16le'/'ucs2', 'base64', 'base64url', 'latin1'/'binary', 'hex'
		switch (forceCharset || contentTypeHeader.charset) {
			case "utf8":
				charset = "utf-8";
				break;
			case "iso-8859-1":
				charset = "latin1";
				break;
		}
	}

	// Decode the data charset
	const decoded = body.toString(charset);

	// Parse the data
	switch (contentType) {
		case "json":
			return JSON.parse(decoded);
		case "text":
			return decoded;
	}
	return body;
}

/// Data structure of bzd nodes is as follow:
/// The following is key-ed by UID:
/// 	pending: [] // an ordered list of pending actions.
/// 	status: {} // a dictionary of well defined statuses (health status) of the node.
///			- cpus
///			- ram
///			- version (SW, HW, ...)
///			- ...
/// 	records: {} // a dictionary of record from the target (those are the internal symbols of the application, it can be anything)
///
/// Note: all data should come from the node, no extra data should be available to ensure a single source of truth.
///		For example no config on the health status check, the node should know it.
///
/// Questions:
/// - What to do with firmware and firmware metadata?

export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options, provider, endpoints);
		this.nodes = null;
		provider.addStartProcess(async () => {
			const storage = await makeStorageFromConfig({
				type: "memory",
				name: "nodes",
			});
			this.nodes = new Nodes(storage, options["nodes.handlers"] || {});
			for (const [uid, data] of Object.entries(options["nodes.data"] || {})) {
				const node = await this.nodes.get(uid);
				await node.insert(["data"], data);
			}

			this.setStorage(await StorageBzd.make(this.nodes));
		});

		endpoints.register("get", "/{uid}/{path:*}", async (context) => {
			const isMetadata = Boolean(context.getQuery("metadata", false));
			const maybeCount = context.getQuery("count", null);
			const node = await this.nodes.get(context.getParam("uid"));

			let output = {};
			if (isMetadata) {
				output = Object.assign(output, {
					timestampServer: Date.now(),
				});
			}

			const maybeData =
				maybeCount === null
					? await node.get(Plugin.paramPathToPaths(context.getParam("path")), isMetadata)
					: await node.getValues(Plugin.paramPathToPaths(context.getParam("path")), maybeCount);
			if (maybeData.isEmpty()) {
				context.sendStatus(404);
				return;
			}

			output = Object.assign(output, {
				data: maybeData.value(),
			});
			context.sendJson(output);
			context.sendStatus(200);
		});

		endpoints.register("post", "/{uid}/{path:*}", async (context) => {
			const data = rawBodyParse(context.getBody(), (name) => context.getHeader(name));
			const node = await this.nodes.get(context.getParam("uid"));
			await node.insert(Plugin.paramPathToPaths(context.getParam("path")), data);
			context.sendStatus(200);
		});
	}

	static paramPathToPaths(paramPath) {
		return paramPath.split("/").map((x) => decodeURIComponent(x));
	}
}
