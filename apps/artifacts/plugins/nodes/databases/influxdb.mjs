import Database from "#bzd/apps/artifacts/plugins/nodes/databases/database.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "database", "influxdb");

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
	static fromKeyToField(key) {
		return key.map((part) => part.replace(/[\s"']+/g, "-")).join(".");
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

	async onRecords(records) {
		let content = [];
		for (const [uid, key, value, timestamp] of records) {
			const timestampNanoseconds = timestamp * 1000000;
			for (const field of DatabaseInfluxDB.fromValueToFields(DatabaseInfluxDB.fromKeyToField(key), value)) {
				content.push(uid + " " + field + " " + timestampNanoseconds);
			}
		}

		const lineProtocol = content.join("\n");
		await this.clientStore.post("/api/v2/write", {
			data: lineProtocol,
		});

		return {
			lineProtocolSize: lineProtocol.length,
		};
	}

	/// Get external data on demand.
	async onExternal(uid, key, count, after, before) {
		const field = DatabaseInfluxDB.fromKeyToField(key);

		let influxQL = "";

		if (after !== null && before !== null) {
			const periodMs = Math.round((before - after) / count) || 1;
			influxQL += 'SELECT FIRST("' + field + '") ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "WHERE time > " + after + "ms AND time < " + before + "ms ";
			influxQL += "GROUP BY time(" + periodMs + "ms)\n";
		} else if (after !== null) {
			influxQL += 'SELECT "' + field + '" ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "WHERE time > " + after + "ms ";
			influxQL += "LIMIT " + count + "\n";
		} else if (before !== null) {
			influxQL += 'SELECT "' + field + '" ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "WHERE time < " + before + "ms ";
			influxQL += "ORDER BY time DESC ";
			influxQL += "LIMIT " + count + "\n";
		} else {
			influxQL += 'SELECT "' + "dsdswe" + '" ';
			influxQL += 'FROM "' + uid + '" ';
			influxQL += "ORDER BY time DESC ";
			influxQL += "LIMIT " + count + "\n";
		}

		const result = await this.clientQuery.post("/query", {
			data: influxQL,
		});

		const data = result.results[0]?.series?.[0]?.values ?? [];
		const output = data
			.filter(([_, value]) => value !== null)
			.map(([timestamp, value]) => {
				return [timestamp, DatabaseInfluxDB.fromDBValueToValue(value)];
			});

		return output.reverse();
	}
}
