import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import SnmpNative from "net-snmp";

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
			case SnmpNative.ObjectType.Counter:
				return data.value;
			case SnmpNative.ObjectType.Counter64: {
				let view = new DataView(new ArrayBuffer(8));
				const length = data.value.length;
				for (let i = 0; i < length; i++) {
					view.setInt8(length - i - 1, data.value[i]);
				}
				return view.getBigUint64(0, /*littleEndian*/ true).toString();
			}
			case SnmpNative.ObjectType.Opaque:
				// http://www.net-snmp.org/docs/mibs/NET-SNMP-TC.txt
				// BER-encoded data / type / length / value
				// Float type
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
				} else {
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
		} finally {
			this.close();
		}
	}

	close() {
		this.session.close();
	}
}

export default class SNMP {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		cache.register("snmp.oid", async (host, version, community, oids, ttl, previous, options) => {
			// Update the time in ms.
			options.timeout = ttl * 1000;

			// Get the data
			const snmp = new Snmp(host, version, community);
			return await snmp.getAndClose(oids);
		});

		cache.register(
			"snmp.oidsByTtl",
			async (snmpArray) => {
				const updateObj = (obj, oid, ttl) => {
					oid = Snmp.normalizeOid(oid);
					obj[oid] = Math.min(obj[oid] || Number.MAX_VALUE, ttl);
				};

				const oidTtlMap = snmpArray.reduce((obj, item) => {
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
				return Object.keys(oidTtlMap).reduce((obj, oid) => {
					const ttl = oidTtlMap[oid];
					obj[ttl] = obj[ttl] || [];
					obj[ttl].push(oid);
					return obj;
				}, {});
			},
			{ timeout: 60 * 60 * 1000 }, // 1h
		);
	}

	async fetch(cache) {
		const oidsByTtl = await cache.get("snmp.oidsByTtl", this.config["snmp.array"] || []);

		let promises = [];
		for (const ttl in oidsByTtl) {
			const oids = oidsByTtl[ttl];
			const promise = cache.get(
				"snmp.oid",
				this.config["snmp.host"],
				this.config["snmp.version"],
				this.config["snmp.community"],
				oids,
				ttl,
			);
			promises.push(promise);
		}

		const values = (await Promise.all(promises)).reduce((obj, result) => {
			return Object.assign(obj, result);
		}, {});

		// Map oids to name and perform the operations
		const results = (this.config["snmp.array"] || []).map((item) => {
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
	}
}
