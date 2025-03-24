import Sink from "#bzd/apps/artifacts/plugins/nodes/sinks/sink.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("artifacts", "nodes", "sink", "influxdb");

export default class SinkInfluxDB extends Sink {
	constructor(...args) {
		super(...args);

		Exception.assert("host" in this.options, "The 'host' attribute must be set.");
		Exception.assert("org" in this.options, "The 'org' attribute must be set.");
		Exception.assert("bucket" in this.options, "The 'bucket' attribute must be set.");
		Exception.assert("token" in this.options, "The 'token' attribute must be set.");

		this.client = new this.components.HttpClientFactory(this.options.host, {
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
	}

	/// Convert a value into influxdb fields.
	static fromValueToFields(key, value) {
		if (typeof value === "number") {
			return [key + "=" + value];
		} else if (typeof value === "boolean") {
			return [key + "=" + (value ? "true" : "false")];
		} else if (typeof value === "string") {
			return [key + "=" + JSON.stringify(value)];
		} else if (Array.isArray(value)) {
			return value.reduce((acc, v, i) => acc.concat(SinkInfluxDB.fromValueToFields(key + "[" + i + "]", v)), []);
		} else if (value === null) {
			return [];
		} else if (typeof value === "object") {
			return Object.entries(value).reduce(
				(acc, [k, v]) => acc.concat(SinkInfluxDB.fromValueToFields(key + "." + k, v)),
				[],
			);
		}
		return [];
	}

	async onRecords(records) {
		let content = [];
		for (const [uid, key, value, timestamp] of records) {
			const timestampNanoseconds = timestamp * 1000000;
			for (const field of SinkInfluxDB.fromValueToFields(
				key.map((part) => part.replace(/[\s"']+/g, "-")).join("."),
				value,
			)) {
				content.push(uid + " " + field + " " + timestampNanoseconds);
			}
		}

		const lineProtocol = content.join("\n");
		await this.client.post("/api/v2/write", {
			data: lineProtocol,
		});

		return {
			lineProtocolSize: lineProtocol.length,
		};
	}
}
