import StorageBzd from "#bzd/apps/artifacts/plugins/nodes/storage.mjs";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records.mjs";
import { HttpClientFactory } from "#bzd/nodejs/core/http/client.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";
import Process from "#bzd/nodejs/core/services/process.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";

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

class FetchFromRemoteProcess extends Process {
	/// Async process to fetch data from a remote.
	///
	/// \param client The http client factory to fetch data from.
	/// \param storageName The name of the storage to be used for the records.
	constructor(plugin, client, storageName) {
		super();
		this.plugin = plugin;
		this.client = client;
		this.storageName = storageName;
		this.tick = 0;
	}

	async process(options) {
		try {
			const tickRemote = this.plugin.records.getTickRemote(this.storageName, 0);
			const result = await this.client.get("/@records", {
				query: {
					tick: tickRemote,
				},
			});

			const timestampRemote = result.timestamp;
			const timestampLocal = Data.getTimestamp();
			const tick = result.next;
			const end = result.end;

			Exception.assert(
				result.version === Plugin.version,
				"Invalid version for remote record: {} vs {} (tick remote: {})",
				result.version,
				Plugin.version,
				tickRemote,
			);
			const updatedRecords = result.records.map((record) =>
				Plugin.recordFromDisk(record).map(([uid, key, value, timestamp]) => {
					return [uid, key, value, timestamp - timestampRemote + timestampLocal];
				}),
			);

			// Apply the records from remote.
			for (const record of updatedRecords) {
				await this.plugin.nodes.insertRecord(record);
				await this.plugin.records.write(Plugin.recordToDisk(record), this.storageName, tick);
			}

			// Update the options.
			if (end === false) {
				options.periodS = 0.1;
			} else {
				const periodS = 600 * Math.exp(-result.records.length / 20);
				options.periodS = Math.min(periodS, 60);
			}

			return {
				tick: [tickRemote, tick],
				end: end,
				nextCallS: options.periodS,
				records: result.records.length,
			};
		} catch (e) {
			options.periodS = 5 * 60; // Retry in 5min on error.
			throw e;
		}
	}
}

export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints, components = {}) {
		// Components that can be mocked for testing.
		components = Object.assign(
			{
				HttpClientFactory: HttpClientFactory,
			},
			components,
		);

		super(volume, options, provider, endpoints);
		this.nodes = null;

		// nodes.records Should look like this:
		// {
		// 	"nodes": {
		// 		"host": "http://localhost:8081",
		//      "volume": "nodes"
		// 	},
		// 	...
		// }
		const optionsRemotes = options["nodes.remotes"] || {};
		const optionsRecords = Object.assign(
			{
				// Set the default storages from the remotes options.
				storages: [Records.defaultStorageName, ...Object.keys(optionsRemotes)],
			},
			options["nodes.records"] || {},
		);

		this.records = new Records(optionsRecords);
		provider.addStartProcess(async () => {
			this.nodes = new Nodes(options["nodes.handlers"] || {});
			for (const [uid, data] of Object.entries(options["nodes.data"] || {})) {
				const node = await this.nodes.get(uid);
				await node.insert(["data"], data);
			}

			this.setStorage(await StorageBzd.make(this.nodes));

			const output = await this.records.init(async (record) => {
				await this.nodes.insertRecord(Plugin.recordFromDisk(record));
			});

			return output;
		});

		// Add fetch processes for remotes.
		//
		//	"nodes": {
		//		"host": "http://localhost:8081"
		//	},
		//  ...
		for (const [storageName, data] of Object.entries(optionsRemotes)) {
			const volume = data.volume || storageName;
			let optionsClient = {
				expect: "json",
			};
			if (data.token) {
				optionsClient["query"] = {
					t: data.token,
				};
			}
			const client = new components.HttpClientFactory(data.host + "/x/" + volume, optionsClient);
			provider.addTimeTriggeredProcess("remote." + storageName, new FetchFromRemoteProcess(this, client, storageName), {
				policy: data.throwOnFailure ? Services.Policy.throw : Services.Policy.ignore,
				periodS: 5,
				delayS: data.delayS || null,
			});
		}

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

		/// Get the records.
		///
		/// GET <endpoint>/@records?tick=10
		/// ```{
		///    timestamp: xxx,   # node current timestamp for reference.
		///    records: [...],   # current records from the specified tick.
		///    next: xxxx        # the tick to provide for the next records.
		///    end : true|false  # if it contains all remaining records or not.
		/// }```
		endpoints.register("get", "/@records", async (context) => {
			// Important note, if tick is greater than this.records.tick, it means that this tick was
			// taken from a previous version/iteration, therefore we consider the tick passed invalid
			// and reset it to 0. This is to handle updates on the remote side.
			let tick = context.getQuery("tick", 0, parseInt);
			if (tick > this.records.tick + 1) {
				tick = 0;
			}
			let maxSize = context.getQuery("size", 1024 * 1024, parseInt);

			let records = [];
			let currentTick = null;
			let record = null;
			let end = true;
			let size = null;
			for await ([currentTick, record, size] of this.records.read(tick)) {
				// Pass the record as written to disk (don't use recordFromDisk)
				// This is done to save network bandwidth.
				records.push(record);
				maxSize -= size;
				if (maxSize <= 0) {
					end = false;
					break;
				}
			}

			const output = {
				version: Plugin.version,
				timestamp: Date.now(),
				records: records,
				next: currentTick === null ? tick : currentTick + 1,
				end: end,
			};
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

			let records = [];
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
					records = records.concat(await node.insert(key, value, timestamp - inputs.data.timestamp));
				}
			} else {
				records = await node.insert(key, inputs.data);
			}

			// Save the data written on disk.
			await this.records.write(Plugin.recordToDisk(records));

			context.sendStatus(200);
		});
	}

	static get version() {
		return 3;
	}

	/// Write a record to the disk.
	///
	/// The process is made to reduce disk space.
	///
	/// \param record The original record.
	///
	/// \return The disk optimized record.
	static recordToDisk(record) {
		let clusters = {};
		for (const [uid, key, value, timestamp] of record) {
			const keyCluster = uid + "@" + timestamp;
			clusters[keyCluster] ??= [];
			clusters[keyCluster].push([key, value]);
		}
		let onDiskRecord = [];
		for (const [keyCluster, cluster] of Object.entries(clusters)) {
			const [uid, timestampStr] = keyCluster.split("@");
			let valueCluster = {};
			for (const [key, value] of cluster) {
				let current = valueCluster;
				for (const part of key) {
					current[part] ??= {};
					current = current[part];
				}
				current["_"] = value;
			}
			onDiskRecord.push([uid, valueCluster, parseInt(timestampStr)]);
		}
		return onDiskRecord;
	}

	/// Read a record form the disk.
	///
	/// \param record The disk optimized record.
	///
	/// \return The original record.
	static recordFromDisk(record) {
		let fromDiskRecord = [];
		const traverse = (fragment, key = [], depth = 0) => {
			Exception.assert(depth < 32, "Recursive function depth exceeded: {}", depth);
			let paths = [];
			for (const [part, data] of Object.entries(fragment)) {
				if (part == "_") {
					paths.push([key, data]);
				} else {
					paths = paths.concat(traverse(data, [...key, part], depth + 1));
				}
			}
			return paths;
		};
		for (const [uid, valueCluster, timestamp] of record) {
			for (const [key, value] of traverse(valueCluster)) {
				fromDiskRecord.push([uid, key, value, timestamp]);
			}
		}
		return fromDiskRecord;
	}

	static paramPathToKey(paramPath) {
		return paramPath
			.split("/")
			.filter(Boolean)
			.map((x) => decodeURIComponent(x));
	}
}
