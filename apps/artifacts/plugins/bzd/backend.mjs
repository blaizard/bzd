import StorageBzd from "#bzd/apps/artifacts/plugins/bzd/storage.mjs";
import Nodes from "#bzd/apps/artifacts/plugins/bzd/nodes.mjs";
import makeStorageFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";

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

function paramPathToPaths(paramPath) {
	return paramPath.split("/").map((x) => decodeURIComponent(x));
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
export default {
	services: {
		nodes: {
			async is(params) {
				return true;
			},
			async start(params, context) {
				const storage = await makeStorageFromConfig({
					type: "memory",
					name: "nodes",
				});
				const nodes = new Nodes(storage);
				for (const [uid, data] of Object.entries(params["bzd.data"] || {})) {
					const node = await nodes.get(uid);
					await node.insert(data, "data");
				}
				return nodes;
			},
			async stop(server) {},
		},
	},
	endpoints: {
		"/{uid}/{path:*}": {
			get: {
				async handler(params, services) {
					const node = await services.nodes.get(params.uid);
					return await node.get(...paramPathToPaths(params.path));
				},
			},
			post: {
				async handler(params, services) {
					const data = rawBodyParse(this.getBody(), (name) => this.getHeader(name));
					const node = await services.nodes.get(params.uid);
					return await node.insert(data, ...paramPathToPaths(params.path));
				},
			},
		},
	},
	async storage(params, services) {
		return await StorageBzd.make(services.nodes);
	},
};
