import StorageBzd from "#bzd/apps/artifacts/plugins/nodes/storage.js";
import { Nodes } from "#bzd/apps/artifacts/plugins/nodes/nodes.js";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import RecordsDistributed from "#bzd/apps/artifacts/plugins/nodes/records/distributed.js";
import { HttpClientFactory } from "#bzd/nodejs/core/http/client.js";
import Services from "#bzd/nodejs/core/services/services.js";
import DatabaseInfluxDB from "#bzd/apps/artifacts/plugins/nodes/databases/influxdb.js";
import SourceNodes from "#bzd/apps/artifacts/plugins/nodes/sources/nodes.js";
import { isObject } from "#bzd/nodejs/utils/object.js";
import Router from "#bzd/nodejs/core/router.js";
import format from "#bzd/nodejs/core/format.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";
import DataGenerator from "#bzd/nodejs/db/data/generator.js";
import { Readable } from "stream";
import MCPServer from "#bzd/nodejs/core/mcp/server.js";
import { timestampMs } from "#bzd/nodejs/utils/timestamp.js";
import { handleDataGet, getDataGetInputsFromQuery } from "#bzd/nodejs/db/data/backend/handler.js";

const databaseTypes = {
	influxdb: DatabaseInfluxDB,
};

const sourceTypes = {
	nodes: SourceNodes,
};

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

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

/// Build a router for the dashboards.
///
/// The router will match the inputs of the dashboards.
function buildRouterForDashboards(optionsDashboards) {
	// Group all matches together, there might be multiple dashboards with the same inputs.
	let matches = new Set();
	for (const dashboard of optionsDashboards) {
		Exception.assert("title" in dashboard, "Dashboard must have a title: {:?}", dashboard);
		Exception.assert("inputs" in dashboard, "Dashboard must have inputs: {:?}", dashboard);
		for (const [match, _] of Object.entries(dashboard.inputs)) {
			matches.add(match);
		}
	}

	// Create a router to match the inputs.
	const router = new Router();
	for (const match of matches) {
		router.add(match, () => {}, match);
	}

	return router;
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
		let dbReadExternal = null;

		// nodes.records Should look like this:
		// {
		// 	"nodes": {
		// 		"host": "http://localhost:8081",
		//      "volume": "nodes"
		// 	},
		// 	...
		// }
		const optionsSources = options["nodes.sources"] || {};
		const optionsDatabases = options["nodes.databases"] || {};
		const optionsSchema = options["nodes.schema"] || {};
		const optionsHandlers = options["nodes.handlers"] || {};

		// Add the validation to the handlers options.
		for (const [path, validation] of Object.entries(optionsSchema)) {
			optionsHandlers[path] ??= {};
			optionsHandlers[path].validation = validation;
		}

		const recordsMainStorageName = "main";
		const optionsRecords = Object.assign(
			{
				// Set the default storages from the remotes options.
				storages: [recordsMainStorageName, ...Object.keys(optionsSources)],
				statistics: this.statistics,
				locks: options.locks,
			},
			options["nodes.records"] || {},
		);
		const optionsDashboards = options["nodes.dashboards"] || [];
		const routerDashboards = buildRouterForDashboards(optionsDashboards);

		this.records = new RecordsDistributed(optionsRecords);
		provider.addStartProcess("records.initialize", async () => {
			let optionsNodes = {
				cache: this.cache,
				statistics: this.statistics.makeNested("data"),
			};
			if (dbReadExternal !== null) {
				optionsNodes["external"] = async (...args) => {
					return await dbReadExternal.onExternal(...args);
				};
			}

			this.nodes = new Nodes(optionsHandlers, optionsNodes);
			for (const [uid, data] of Object.entries(options["nodes.data"] || {})) {
				await this.nodes.insert(uid, ["data"], data);
			}

			this.setStorage(await StorageBzd.make(this.nodes));

			const output = await this.records.init(provider, async (record) => {
				this.nodes.insertFromRecord(Nodes.recordFromDisk(record));
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
			Log.info("[{}] Using source '{}'.", volume, storageName);
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

		// Add database processes.
		//
		// "name": {
		//     "type": "influxdb",
		//     "host": "http://localhost:8081"
		// }
		for (const [databaseName, databaseOptions] of Object.entries(optionsDatabases)) {
			Exception.assert(databaseOptions.type in databaseTypes, "Unrecognized database type '{}'.", databaseOptions.type);
			Log.info(
				"[{}] Using database '{}' with read={}/write={}.",
				volume,
				databaseName,
				databaseOptions.read || false,
				databaseOptions.write || false,
			);
			const database = new databaseTypes[databaseOptions.type](this, databaseOptions, components);
			const providerDatabase = provider.makeNested("database", databaseName);
			database.installServices(providerDatabase);

			if (databaseOptions.read) {
				Exception.assert(
					dbReadExternal === null,
					"[{}] database read is already set, only one can be set at a time.",
					volume,
				);
				dbReadExternal = database;
			}
		}

		/// Endpoint to perform time synchronization.
		///
		/// This is used to get the time from the server and measurte the time
		/// delay between the client and the server clock.
		/// It can be measured as follow:
		/// ~ ((timestamp - t1) + (t4 - timestamp)) / 2
		endpoints.register("get", "/@time", async (context) => {
			context.setStatus(200);
			context.sendJson({
				version: this.version,
				timestamp: timestampMs(),
			});
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
						timestamp: timestampMs(),
					},
					output,
				),
			);
		});

		/// Export a dataset into a file of a specific format.
		endpoints.register("get", "/@export/{uid}/{path:*}", async (context) => {
			const uid = context.getParam("uid");
			const key = Utils.pathToKey(context.getParam("path"));
			const format = context.getQuery("format", "csv", String);
			const children = context.getQuery("children", 0, parseInt);
			const after = context.getQuery("after", 0, parseInt);
			const before = context.getQuery("before", null, parseInt);

			const generator = new DataGenerator(this.nodes.data, uid, key, children, after, before);
			const name = [uid, ...key].join(".").replace(/[^a-z0-9_\-\.]+/gi, "-");

			switch (format) {
				case "csv":
					context.setHeader("Content-Type", "text/csv");
					context.setHeader("Content-Disposition", 'attachment; filename="' + name + '.csv"');
					const stream = new Readable({ read() {} });
					context.sendStream(stream);

					const columns = await generator.getColumns();
					stream.push("date;timestamp;" + columns.join(";") + "\n");

					const valueToCell = (value) => {
						switch (typeof value) {
							case "undefined":
								return "";
							case "object":
								return JSON.stringify(value);
							case "boolean":
								return value ? "true" : "false";
							default:
								return String(value);
						}
					};

					for await (const [timestamp, values] of generator.byTimestamp()) {
						const row = columns.map((column) => valueToCell(values[column]));
						stream.push(new Date(timestamp).toUTCString() + ";" + timestamp + ";" + row.join(";") + "\n");
					}
					stream.push(null);
					break;
				default:
					Exception.assertPrecondition(false, "Unsupported format '{}'.", format);
			}
		});

		/// Get information about the dashboards at the specified path.
		endpoints.register("get", "/@dashboards/{uid}/{path:*}", async (context) => {
			const uid = context.getParam("uid");
			const rootKey = Utils.pathToKey(context.getParam("path"));
			const children = await this.nodes.getChildren({ uid: uid, key: rootKey, children: 99, includeInner: false });

			// Go through the children and match them against the router.
			let inputs = {};
			if (children) {
				for (const { key } of children) {
					const path = Utils.keyToPath([...rootKey, ...key]);
					const match = routerDashboards.match(path);
					if (match) {
						inputs[match.args] ??= [];
						inputs[match.args].push(match.vars);
					}
				}
			}

			let dashboardsByUid = {};
			optionsDashboards.forEach((dashboard, index) => {
				for (const [match, options] of Object.entries(dashboard.inputs)) {
					for (const vars of inputs[match] || []) {
						// Generate the UID by index and some variables. Exclude the group by, as we want
						// to keep these indexes together when they diverge.
						const filtereVars = Object.keys(vars).reduce((acc, key) => {
							if (!(dashboard.groupBy || []).includes(key)) {
								acc[key] = vars[key];
							}
							return acc;
						}, {});
						const uid = index + "-" + JSON.stringify(filtereVars);

						dashboardsByUid[uid] ??= { dashboard: dashboard, inputs: {}, vars: filtereVars };
						const varsEncoded = Object.fromEntries(Object.entries(vars).map(([k, v]) => [k, encodeURIComponent(v)]));
						const path = format(match, varsEncoded);
						dashboardsByUid[uid].inputs[path] = options;
					}
				}
			});

			// Updated some fields with the actual ones and return the dashboards.
			const dashboards = Object.values(dashboardsByUid).map((entry) => {
				return Object.assign({}, entry.dashboard, {
					inputs: entry.inputs,
					title: format(entry.dashboard.title, entry.vars),
				});
			});

			context.setStatus(200);
			context.sendJson({
				version: this.version,
				dashboards: dashboards,
				timestamp: timestampMs(),
			});
		});

		/// Retrieve values from the nodes.
		endpoints.register("get", "/{uid}/{path:*}", async (context) => {
			const uid = context.getParam("uid");
			const key = Utils.pathToKey(context.getParam("path"));

			const inputs = getDataGetInputsFromQuery(context);
			const maybeOuput = await handleDataGet(this.nodes, Object.assign({ uid, key }, inputs));

			if (maybeOuput === null) {
				context.sendStatus(404);
			} else {
				context.setStatus(200);
				context.sendJson(maybeOuput);
			}
		});

		/// Insert one or multiple entries.
		///
		/// \note If <path:*> is not empty the first item is the uid, while the rest corresponds to the key.
		/// If <path:*> is empty, value is expected to be a dictionary which keys are the uid and value the actual value.
		///
		/// POST <endpoint>/<path:*>
		/// ```<value>``` -> stores a new value to the path at the server timestamp.
		///
		/// POST <endpoint>/<path:*>?bulk=1
		/// ```{
		///    timestamp: xxx,             # Node current timestamp for reference.
		///                                # If unset, the timestamp will be treated as "fixed timestamps".
		///    data: [
		///      [<timestamp1>, <value1>], # Store a new value with timestamp1.
		///      [<timestamp2>, <value2>]  # Store another value with timestamp2.
		///    ]
		/// }```
		endpoints.register("post", "/{path:*}", async (context) => {
			let inputs = {};
			try {
				inputs.bulk = context.getQuery("bulk", false, Boolean);
				inputs.data = rawBodyParse(context.getBody(), (name) => context.getHeader(name));
			} catch (e) {
				context.sendStatus(400, String(e));
				return;
			}

			const now = timestampMs();
			// Normalize the data to bulk data format.
			// bulk = List[Tuple[<timestamp>, <values>]]
			// timestampClient = The timestamp on the client side, if unset, use the current timestamp.
			// isFixedTimestamp = If this timestamp is intended to be modified or not.
			const [bulk, timestampClient, isFixedTimestamp] = ((data) => {
				if (inputs.bulk) {
					const isFixedTimestamp = !("timestamp" in data);
					Exception.assertPrecondition(isObject(data), "The data must be an object: {:?}", data);
					Exception.assertPrecondition(
						isFixedTimestamp || typeof data.timestamp == "number",
						"The timestamp given is not a number {}.",
						data.timestamp,
					);
					return [data.data, data.timestamp ?? now, isFixedTimestamp];
				} else {
					return [[[now, inputs.data]], now, false];
				}
			})(inputs.data);

			let records = [];
			const [uid, ...key] = Utils.pathToKey(context.getParam("path"));

			for (const [timestamp, value] of bulk) {
				Exception.assertPrecondition(
					typeof timestamp == "number",
					"The timestamp given for value '{:?}' is not a number {}.",
					value,
					timestamp,
				);
				const actualTimestamp = isFixedTimestamp ? timestamp : timestamp - timestampClient + timestampMs();

				// If there is no 'uid' set for this request, it must be embedded in the value.
				if (!uid) {
					Exception.assertPrecondition(isObject(value), "Expected an object, containing uid to values: {:?}", value);
					for (const [uid, subValue] of Object.entries(value)) {
						records = records.concat(await this.nodes.insert(uid, key, subValue, actualTimestamp, isFixedTimestamp));
					}
				} else {
					records = records.concat(await this.nodes.insert(uid, key, value, actualTimestamp, isFixedTimestamp));
				}
			}

			// Save the data written on disk.
			await this.records.write(Nodes.recordToDisk(records), recordsMainStorageName);

			context.sendStatus(200);
		});

		for (const [endpoint, mcpOptions] of Object.entries(options["nodes.mcp"] || {})) {
			const schema = MCPServer.updateSchema(
				{
					tools: {
						list_nodes: {
							description: "Get a list of all available nodes.",
						},
						get: {
							description: "Get data attached to a node. Returns {data: null} if the node does not exist.",
							parameters: {
								type: "object",
								properties: {
									name: {
										description: "The exact name of the node as provided by 'list_nodes' to get the data for.",
										type: "string",
									},
									key: {
										description:
											"The key to target, to be use if we do not want to access the full tree for a specific node.",
										type: "array",
										items: {
											type: "string",
										},
										default: [],
									},
									metadata: {
										description:
											"If true, return [timestamp, value, (expiry time in seconds), (unit)] tuples for each entry along with a server-reference timestamp.",
										type: "boolean",
										default: false,
									},
									children: {
										description: "Number of nested levels to include as children.",
										type: "integer",
										minimum: 0,
										default: 99,
									},
									count: {
										description: "Maximum number of values to return per entry.",
										type: "integer",
										minimum: 0,
									},
									after: {
										description: "Only return values whose timestamp is strictly after this value.",
										type: "integer",
									},
									before: {
										description: "Only return values whose timestamp is strictly before this value.",
										type: "integer",
									},
									include: {
										description:
											"List of key paths (each path being a list of segments) to restrict the output to, e.g. [['a','b']].",
										type: "array",
										items: {
											type: "array",
											items: { type: "string" },
										},
									},
									sampling: {
										description: "Sampling method to apply when aggregating returned values.",
										type: "string",
									},
									keys: {
										description: "Return the list of child keys as objects of the form {key: [...], leaf: boolean}.",
										type: "boolean",
										default: false,
									},
								},
								required: ["name"],
								additionalProperties: false,
							},
						},
						schema: {
							description: "Get the description and schema of the data available.",
						},
					},
				},
				mcpOptions || {},
			);
			endpoints.registerMCP(
				endpoint,
				async (tool, args) => {
					switch (tool) {
						case "list_nodes":
							let uids = [];
							for await (const uid of this.nodes.getNodes()) {
								uids.push(uid);
							}
							return uids;
						case "get": {
							const maybeOutput = await handleDataGet(this.nodes, {
								uid: args.name.trim(),
								key: args.key,
								metadata: args.metadata,
								children: args.children,
								count: args.count,
								after: args.after,
								before: args.before,
								include: args.include,
								sampling: args.sampling,
							});
							if (maybeOutput === null) {
								return { data: null };
							}
							return maybeOutput;
						}
						case "schema":
							return optionsSchema;
						default:
							Exception.unreachable("Unsupported tool '{}'", tool);
					}
				},
				schema,
			);
		}
	}

	get version() {
		return 4;
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

	/// Write a value to the in-memory data.
	///
	/// Note, it does not:
	/// - Write a record for this.
	/// - Use the handlers mechanism.
	///
	/// \param uid The identifier of the node.
	/// \param data The data to be written.
	/// \param timestamp The timestamp in Ms of this value.
	write(uid, data, timestamp) {
		this.nodes.insertFromRecord(Nodes.recordFromSingleEntry(uid, data, timestamp));
	}
}
