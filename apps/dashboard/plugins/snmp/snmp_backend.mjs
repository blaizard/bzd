import SnmpNative from "net-snmp";
import ExceptionFactory from "bzd/core/exception.mjs";
import LogFactory from "bzd/core/log.mjs";

const Exception = ExceptionFactory("snmp");
const Log = LogFactory("snmp");

class Snmp {
	constructor(host, version, community) {
		const versionMapping = {
			1: SnmpNative.Version1,
			"2c": SnmpNative.Version2c,
			3: SnmpNative.Version3,
		};
		Exception.assert(version in versionMapping, "Unsupported version '{}'", version);
		this.session = new SnmpNative.createSession(host, community, {
			version: versionMapping[version],
		});
	}

	static _castValue(data) {
		switch (data.type) {
		case SnmpNative.ObjectType.OctetString:
			return data.value.toString();
		case SnmpNative.ObjectType.Integer:
			return data.value;
		case SnmpNative.ObjectType.Opaque:
			/*
			 * http://www.net-snmp.org/docs/mibs/NET-SNMP-TC.txt
			 * BER-encoded data / type / length / value
			 * Float type
			 */
			if (data.value[0] == 0x9f && data.value[1] == 0x78) {
				const length = data.value[2];
				let view = new DataView(new ArrayBuffer(length));
				for (let i = 0; i < length; i++) {
					view.setInt8(i, data.value[3 + i]);
				}
				switch (length) {
				case 4:
					return view.getFloat32(0);
				}
			}
			break;
		}
		Log.error("Unsupported value type '{}': {:j}", data.type, data);
		return "unsupported";
	}

	static normalizeOid(oid) {
		return String(oid)
			.split(".")
			.filter((item) => Boolean(item))
			.join(".");
	}

	static isOid(value) {
		return /^\.?([0-9]+\.){2,}[0-9]+$/.test(value);
	}

	async get(oids) {
		return new Promise((resolve, reject) => {
			this.session.get(oids, (error, varbinds) => {
				if (error) {
					reject(new Exception(error));
				}
				else {
					let result = {};
					for (const i in varbinds) {
						if (SnmpNative.isVarbindError(varbinds[i])) {
							reject(new Exception(SnmpNative.varbindError(varbinds[i])));
							return;
						}
						result[varbinds[i].oid] = Snmp._castValue(varbinds[i]);
					}
					resolve(result);
				}
			});
		});
	}

	async getAndClose(oids) {
		try {
			return await this.get(oids);
		}
		finally {
			this.close();
		}
	}

	close() {
		this.session.close();
	}
}

export default {
	cache: [
		{
			collection: "snmp.oid",
			fetch: async (host, version, community, oids, ttl, previous, options) => {
				// Update the time in ms.
				options.timeout = ttl * 1000;

				// Get the data
				const snmp = new Snmp(host, version, community);
				return await snmp.getAndClose(oids);
			},
		},
	],
	constructor: async (data) => {
		const updateObj = (obj, oid, ttl) => {
			oid = Snmp.normalizeOid(oid);
			obj[oid] = Math.min(obj[oid] || Number.MAX_VALUE, ttl);
		};

		const oidTtlMap = (data["snmp.array"] || []).reduce((obj, item) => {
			const ttl = item.ttl || 2000;
			updateObj(obj, item.oid, ttl);
			(item.ops || [])
				.filter((item) => Snmp.isOid(item.value))
				.forEach((item) => {
					updateObj(obj, item.value, ttl);
				});
			return obj;
		}, {});

		// Sort the Oids by ttl
		data.oidsByTtl = Object.keys(oidTtlMap).reduce((obj, oid) => {
			const ttl = oidTtlMap[oid];
			obj[ttl] = obj[ttl] || [];
			obj[ttl].push(oid);
			return obj;
		}, {});

		return data;
	},
	fetch: async (data, cache) => {
		Exception.assert("oidsByTtl" in data, "Seems that the constructor was not called.");

		let promises = [];
		for (const ttl in data.oidsByTtl) {
			const oids = data.oidsByTtl[ttl];
			const promise = cache.get("snmp.oid", data["snmp.host"], data["snmp.version"], data["snmp.community"], oids, ttl);
			promises.push(promise);
		}

		const values = (await Promise.all(promises)).reduce((obj, result) => {
			return Object.assign(obj, result);
		}, {});

		// Map oids to name and perform the operations
		const results = (data["snmp.array"] || []).map((item) => {
			const oid = Snmp.normalizeOid(item.oid);
			let result = {
				id: item.id || "unknown",
				value: values[oid],
			};

			(item.ops || []).forEach((operation) => {
				const opValue = Snmp.isOid(operation.value) ? values[Snmp.normalizeOid(operation.value)] : operation.value;
				switch (operation.type) {
				case "mul":
					result.value *= opValue;
					break;
				case "div":
					result.value /= opValue;
					break;
				case "add":
					result.value += opValue;
					break;
				case "sub":
					result.value -= opValue;
					break;
				default:
					Exception.unreachable("Unsupported operation type: '{}'", operation.type);
				}
			});

			return result;
		});

		return results;
	},
};
