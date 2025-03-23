import StorageBzd from "#bzd/apps/artifacts/plugins/nodes/storage.mjs";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Records from "#bzd/apps/artifacts/plugins/nodes/records.mjs";
import { HttpClientFactory } from "#bzd/nodejs/core/http/client.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";
import Process from "#bzd/nodejs/core/services/process.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";
import SinkInfluxDB from "#bzd/apps/artifacts/plugins/nodes/sinks/influxdb.mjs";
import SourceNodes from "#bzd/apps/artifacts/plugins/nodes/sources/nodes.mjs";

const sinkTypes = {
	influxdb: SinkInfluxDB,
};

const sourceTypes = {
	nodes: SourceNodes,
};

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
		const optionsSources = options["nodes.sources"] || {};
		const optionsSinks = options["nodes.sinks"] || {};
		const optionsRecords = Object.assign(
			{
				// Set the default storages from the remotes options.
				storages: [Records.defaultStorageName, ...Object.keys(optionsSources)],
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
				await this.nodes.insertRecord(Nodes.recordFromDisk(record));
			});

			return output;
		});

		// Add source processes.
		//
		//	"storageName": {
		//     "type": "nodes",
		//		"host": "http://localhost:8081"
		//	},
		//  ...
		for (const [storageName, data] of Object.entries(optionsSources)) {
			Exception.assert(data.type in sourceTypes, "Unrecognized source type '{}'.", data.type);
			provider.addTimeTriggeredProcess(
				"source." + storageName,
				new sourceTypes[data.type](this, storageName, data, components),
				{
					policy: data.throwOnFailure ? Services.Policy.throw : Services.Policy.ignore,
					periodS: 5,
					delayS: data.delayS || null,
				},
			);
		}

		// Add sink processes.
		//
		// "name": {
		//     "type": "influxdb",
		//     "host": "http://localhost:8081"
		// }
		for (const [sinkName, data] of Object.entries(optionsSinks)) {
			Exception.assert(data.type in sinkTypes, "Unrecognized sink type '{}'.", data.type);
			provider.addTimeTriggeredProcess("sink." + sinkName, new sinkTypes[data.type](this, data, components), {
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
			// Pass the record as written to disk (don't use recordFromDisk)
			// This is done to save network bandwidth.
			const output = await this.read(tick, maxSize, /*diskFormat*/ true);

			context.setStatus(200);
			context.sendJson(
				Object.assign(
					{
						version: this.version,
						timestamp: Date.now(),
					},
					output,
				),
			);
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
			await this.records.write(Nodes.recordToDisk(records));

			context.sendStatus(200);
		});
	}

	get version() {
		return 3;
	}

	static paramPathToKey(paramPath) {
		return paramPath
			.split("/")
			.filter(Boolean)
			.map((x) => decodeURIComponent(x));
	}

	/// Read records from the given tick.
	///
	/// \param tick The initial tick to read from.
	/// \param maxSize The maximum size the data should be (this is an approximation).
	/// \param diskFormat Whether the record should keep the disk formatting or be converted.
	///
	/// \return A dictionary with the records, the next tick and a flag specifying if this is
	/// the end of the database.
	async read(tick, maxSize, diskFormat) {
		let records = [];
		let currentTick = null;
		let record = null;
		let end = true;
		let size = null;
		for await ([currentTick, record, size] of this.records.read(tick)) {
			records.push(diskFormat ? record : Nodes.recordFromDisk(record));
			maxSize -= size;
			if (maxSize <= 0) {
				end = false;
				break;
			}
		}

		return {
			records: records,
			next: currentTick === null ? tick : currentTick + 1,
			end: end,
		};
	}
}
