import StorageBzd from "#bzd/apps/artifacts/plugins/nodes/storage.mjs";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");

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
		this.records = new Records("records." + volume);
		provider.addStartProcess(async () => {
			this.nodes = new Nodes(options["nodes.handlers"] || {});
			for (const [uid, data] of Object.entries(options["nodes.data"] || {})) {
				const node = await this.nodes.get(uid);
				await node.insert(["data"], data);
			}

			this.setStorage(await StorageBzd.make(this.nodes));

			await this.records.init((payload) => {
				console.log(payload);
			});
		});

		/// Retrieve values from the store.
		///
		/// GET <endpoint>/<uid>/<path:*>
		/// ```{data: <value>}``` -> Return the raw value at the given path.
		///
		/// GET <endpoint>/<uid>/<path:*>?metadata=1
		/// ```{
		///   timestamp: <timestamp>, # The current server timestamp for reference,
		///   data: [<timestamp>, <value>] # The latest value and its timestamp.
		/// }```
		///
		/// GET <endpoint>/<uid>/<path:*>?count=2
		/// ```{
		///   data: [
		///      <value1>, # The latest value.
		///      <value2>  # Another value.
		///   ]
		/// }```
		///
		/// GET <endpoint>/<uid>/<path:*>?children=3
		/// ```{
		///   data: [
		///      [["a", "b"], <value1>],       # The latest value for /<path>/a/b.
		///      [["a", "d", "e"], <value2>]   # The latest value for /<path>/a/d/e.
		///   ]
		/// }```
		///
		/// GET <endpoint>/<uid>/<path:*>?after=<timestamp>
		/// Only show entries after a specific timestamp (not including)
		///
		/// GET <endpoint>/<uid>/<path:*>?before=<timestamp>
		/// Only show entries before a specific timestamp (not including)
		///
		/// GET <endpoint>/<uid>/<path:*>?include=/a/b,/a/d/e
		/// Only show the path /a/b and /a/d/e
		///
		endpoints.register("get", "/{uid}/{path:*}", async (context) => {
			const metadata = context.getQuery("metadata", false, Boolean);
			const children = context.getQuery("children", 0, parseInt);
			const count = context.getQuery("count", null, parseInt);
			const after = context.getQuery("after", null, parseInt);
			const before = context.getQuery("before", null, parseInt);
			const include = context.getQuery("include", null, (value) =>
				value
					.split(",")
					.filter(Boolean)
					.map((path) => Plugin.paramPathToKey(path)),
			);

			const node = await this.nodes.get(context.getParam("uid"));

			let output = {};
			if (metadata) {
				output = Object.assign(output, {
					timestamp: Date.now(),
				});
			}

			const maybeData = await node.get({
				key: Plugin.paramPathToKey(context.getParam("path")),
				metadata,
				children,
				count,
				after,
				before,
				include,
			});
			if (maybeData.isEmpty()) {
				context.sendStatus(404);
				return;
			}

			output = Object.assign(output, {
				data: maybeData.value(),
			});
			context.setStatus(200);
			context.sendJson(output);
		});

		/// Insert one or multiple entries.
		///
		/// POST <endpoint>/<uid>/<path:*>
		/// ```<value>``` -> stores a new value to the path at the server timestamp.
		///
		/// POST <endpoint>/<uid>/<path:*>?bulk=1
		/// ```{
		///    timestamp: xxx,             # node current timestamp for reference.
		///    data: [
		///      [<timestamp1>, <value1>], # Store a new value with timestamp1
		///      [<timestamp2>, <value2>]  # Store another value with timestamp2
		///    ]
		/// }```
		endpoints.register("post", "/{uid}/{path:*}", async (context) => {
			let inputs = {};
			try {
				inputs.bulk = context.getQuery("bulk", false, Boolean);
				inputs.data = rawBodyParse(context.getBody(), (name) => context.getHeader(name));
			} catch (e) {
				context.sendStatus(400, String(e));
				return;
			}
			const node = await this.nodes.get(context.getParam("uid"));
			const key = Plugin.paramPathToKey(context.getParam("path"));

			let written = [];
			if (inputs.bulk) {
				Exception.assertPrecondition(
					typeof inputs.data.timestamp == "number",
					"The timestamp given is not a number {}.",
					inputs.data.timestamp,
				);
				for (const [timestamp, value] of inputs.data.data) {
					Exception.assertPrecondition(
						typeof timestamp == "number",
						"The timestamp given for value '{:j}' is not a number {}.",
						value,
						timestamp,
					);
					written = written.concat(await node.insert(key, value, timestamp - inputs.data.timestamp));
				}
			} else {
				written = await node.insert(key, inputs.data);
			}

			// Save the data written on disk.
			await this.records.write(written);

			context.sendStatus(200);
		});
	}

	static paramPathToKey(paramPath) {
		return paramPath
			.split("/")
			.filter(Boolean)
			.map((x) => decodeURIComponent(x));
	}
}
