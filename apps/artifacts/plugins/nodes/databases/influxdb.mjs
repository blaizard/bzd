import Database from "#bzd/apps/artifacts/plugins/nodes/databases/database.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Utils from "#bzd/apps/artifacts/common/utils.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "database", "influxdb");
const Log = LogFactory("artifacts", "nodes", "database", "influxdb");

export default class DatabaseInfluxDB extends Database {
	constructor(...args) {
		super(...args);

		Exception.assert("host" in this.options, "The 'host' attribute must be set.");
		Exception.assert("org" in this.options, "The 'org' attribute must be set.");
		Exception.assert("bucket" in this.options, "The 'bucket' attribute must be set.");
		Exception.assert("token" in this.options, "The 'token' attribute must be set.");

		this.clientStore = new this.components.HttpClientFactory(this.options.host, {
			query: {
				org: this.options.org,
				bucket: this.options.bucket,
				precision: "ns",
			},
			headers: {
				Authorization: "Token " + this.options.token,
				"Content-Type": "text/plain; charset=utf-8",
			},
			expect: "json",
		});

		this.clientRetention = new this.components.HttpClientFactory(this.options.host, {
			headers: {
				Authorization: "Token " + this.options.token,
				"Content-Type": "application/json",
			},
			expect: "json",
		});

		this.clientQuery = new this.components.HttpClientFactory(this.options.host, {
			query: {
				db: this.options.bucket,
				epoch: "ms",
			},
			headers: {
				"Content-Type": "application/vnd.influxql",
			},
			authentication: {
				type: "basic",
				username: "ignored",
				password: this.options.token,
			},
			expect: "json",
		});

		this.retentionS = this.options.retentionS || null;
	}

	/// Convert a value into influxdb fields.
	static fromValueToFields(key, value) {
		if (typeof value === "number") {
			return [key + "=" + value];
		} else if (typeof value === "boolean") {
			return [key + "=" + (value ? "true" : "false")];
		} else if (typeof value === "string") {
			return [key + "=" + JSON.stringify(JSON.stringify(value))];
		} else if (Array.isArray(value)) {
			return [key + "=" + JSON.stringify(JSON.stringify(value))];
		} else if (value === null) {
			return [];
		} else if (typeof value === "object") {
			return Object.entries(value).reduce(
				(acc, [k, v]) => acc.concat(DatabaseInfluxDB.fromValueToFields(key + "." + k, v)),
				[],
			);
		}
		return [];
	}

	/// Convert a key to a field.
	///
	/// Here are some restrictions:
	/// - You cannot use a comma ,.
	/// - No Leading Underscores.
	/// - Avoid Special Characters.
	/// - Avoid \ to avoid escaping complexity.
	static fromKeyToField(key) {
		return key
			.map((part) =>
				part.replace(/[\s"',=\.\\]/g, (char) => {
					const hexValue = char.charCodeAt(0).toString(16).padStart(2, "0");
					return "|x" + hexValue;
				}),
			)
			.join(".")
			.replace(/^_/, "|x5f");
	}

	/// Convert a field to a key.
	static fromFieldToKey(field) {
		return field.split(".").map((part) =>
			part.replace(/\|x([0-9a-fA-F]{2})/g, (match, hex) => {
				return String.fromCharCode(parseInt(hex, 16));
			}),
		);
	}

	/// Convert a value coming from the database to an actual value.
	static fromDBValueToValue(value) {
		if (typeof value === "string") {
			try {
				return JSON.parse(value);
			} catch (e) {
				// It should not go there, but for now since we have historical data,
				// it can be the case. Remove this after a month.
				return value;
			}
		}
		return value;
	}

	installServices(provider) {
		super.installServices(provider);
		provider.addStartProcess("initialize", async () => {
			return await this.initialize();
		});
		if (this.options.read) {
			provider.addTimeTriggeredProcess(
				"read",
				async (options) => {
					return await this.read(options);
				},
				{
					policy: this.options.throwOnFailure ? Services.Policy.throw : Services.Policy.ignore,
					periodS: 86400,
					delayS: 0,
				},
			);
		}
	}

	async initialize() {
		// If unset try to get the retention from the server.
		if (this.retentionS === null) {
			const response = await this.clientRetention.get("/api/v2/buckets");
			let orgBucket = null;
			for (const bucket of response.buckets) {
				if (bucket.orgID == this.options.org) {
					orgBucket = bucket;
					break;
				}
			}
			Exception.assert(
				orgBucket !== null,
				"Couldn't find bucket corresponding to our organization '{}': {:j}",
				this.options.org,
				response,
			);
			this.retentionS =
				orgBucket.retentionRules.reduce((acc, value) => (value.type == "expire" ? value.everySeconds : acc), 0) || null;
		}

		return {
			retentionS: this.retentionS,
		};
	}

	/// Read all the measurements and all fields and initial values to populate the database.
	///
	/// This is done sequentially to reduce the load, as it doesn't need to be performed fast.
	async read(options) {
		const result = await this.clientQuery.post("/query", {
			data: "SHOW MEASUREMENTS",
		});
		const measurements = result.results[0]?.series?.[0]?.values ?? [];

		// Gather the data.
		const gatherFieldAndValues = async (uid, useFields) => {
			const selector = useFields ? useFields.map((field) => '"' + field + '"').join(",") : "*";
			const result = await this._sql("SELECT " + selector + ' FROM "' + uid + '" ORDER BY time DESC LIMIT 1');
			const data = result?.series?.[0] ?? {};
			const [_time, ...fields] = data.columns;
			const values = data.values
				.map((values) => {
					const timestamp = values[0];
					return fields.map((field, index) => {
						return values[index + 1] === null
							? null
							: [DatabaseInfluxDB.fromFieldToKey(field), values[index + 1], timestamp];
					});
				})
				.flat();
			const remainingFields = fields.filter((_, index) => values[index] === null);
			return [values.filter(Boolean), remainingFields];
		};

		// Gather the data as chunks.
		const gatherViaChunks = async (uid, useFields, chunkSize) => {
			if (useFields === null) {
				return await gatherFieldAndValues(uid, useFields);
			} else {
				let promises = [];
				while (useFields.length) {
					const chunk = useFields.splice(0, chunkSize);
					promises.push(gatherFieldAndValues(uid, chunk));
				}
				const result = await Promise.all(promises);
				return result.reduce(
					([values, remainingFields], [v, r]) => {
						return [
							[...values, ...v],
							[...remainingFields, ...r],
						];
					},
					[[], []],
				);
			}
		};

		// Read all measurements of each uid and their values.
		let nbValues = 0;
		for (const [uid] of measurements) {
			let useFields = null;
			while (useFields === null || useFields.length > 0) {
				const [values, remainingFields] = await gatherViaChunks(uid, useFields, 100);
				nbValues += values.length;
				for (const [key, value, timestamp] of values) {
					await this.plugin.write(uid, key, DatabaseInfluxDB.fromDBValueToValue(value), timestamp);
				}
				useFields = remainingFields;
			}
		}

		return {
			measurements: measurements.length,
			nbValues: nbValues,
		};
	}

	async onRecords(records) {
		let content = [];
		let skipped = 0;
		const timestampMin = this.retentionS ? Utils.timestampMs() - this.retentionS * 1000 + 86400 * 1000 : 0;
		for (const [uid, key, value, timestamp] of records) {
			// InfluxDB doesn't support numbers with comma.
			const timestampNanoseconds = Math.round(timestamp * 1000000);
			for (const field of DatabaseInfluxDB.fromValueToFields(DatabaseInfluxDB.fromKeyToField(key), value)) {
				Exception.assert(
					timestamp,
					"timestamp is null: {} for record: [{}, ..., {}]",
					timestampMin,
					uid,
					timestampNanoseconds,
				);
				if (timestamp <= timestampMin) {
					Log.warning(
						"Skipping record [{}, ..., {}], record is too old for the retention policy ({}s).",
						uid,
						timestampNanoseconds,
						this.retentionS,
					);
					++skipped;
					continue;
				}
				content.push(uid + " " + field + " " + timestampNanoseconds);
			}
		}

		const lineProtocol = content.join("\n");
		try {
			await this.clientStore.post("/api/v2/write", {
				data: lineProtocol,
			});
		} catch (e) {
			throw Exception.fromError(e, "Line Protocol:\n{}", lineProtocol);
		}

		return {
			skipped: skipped,
			lineProtocolSize: lineProtocol.length,
		};
	}

	async _sql(sql) {
		try {
			const result = await this.clientQuery.post("/query", {
				data: sql,
			});
			return result.results[0];
		} catch (e) {
			throw Exception.fromError(e, "SQL:\n{}", sql);
		}
	}

	/// Get external data on demand.
	async onExternal(uid, key, count, after, before) {
		const field = DatabaseInfluxDB.fromKeyToField(key);

		let influxQL = "";

		if (after !== null && before !== null) {
			const periodMs = Math.round((before - after) / count) || 1;
			influxQL += 'SELECT FIRST("' + field + '") ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "WHERE time > " + Math.round(after) + "ms AND time < " + Math.round(before) + "ms ";
			influxQL += "GROUP BY time(" + periodMs + "ms)\n";
		} else if (after !== null) {
			influxQL += 'SELECT "' + field + '" ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "WHERE time > " + Math.round(after) + "ms ";
			influxQL += "LIMIT " + count + "\n";
		} else if (before !== null) {
			influxQL += 'SELECT "' + field + '" ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "WHERE time < " + Math.round(before) + "ms ";
			influxQL += "ORDER BY time DESC ";
			influxQL += "LIMIT " + count + "\n";
		} else {
			influxQL += 'SELECT "' + field + '" ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "ORDER BY time DESC ";
			influxQL += "LIMIT " + count + "\n";
		}

		const result = await this._sql(influxQL);
		const data = result?.series?.[0]?.values ?? [];
		const output = data
			.filter(([_, value]) => value !== null)
			.map(([timestamp, value]) => {
				return [timestamp, DatabaseInfluxDB.fromDBValueToValue(value)];
			});

		return output.reverse();
	}
}
