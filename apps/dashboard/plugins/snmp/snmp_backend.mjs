import SnmpNative from "net-snmp";
import ExceptionFactory from "bzd/core/exception.mjs";

const Exception = ExceptionFactory("snmp");

class Snmp {
	constructor(host, community) {
		this.session = new SnmpNative.createSession(host, community, {
			version: SnmpNative.Version2c,
		});
	}

	static _castValue(data) {
		switch (data.type) {
		case SnmpNative.ObjectType.OctetString:
			return data.value.toString();
		case SnmpNative.ObjectType.Integer:
			return data.value;
		}
		console.log(data);
		Exception.unreachable("Unsupported value type: '{}'.", data.type);
	}

	static normalizeOid(oid) {
		return String(oid).split(".").filter((item) => Boolean(item)).join(".");
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
			fetch: async (host, community, oids, ttl, previous, options) => {
				// Update the time in ms.
				options.timeout = ttl * 1000;

				// Get the data
				const snmp = new Snmp(host, community);
				return await snmp.getAndClose(oids);
			}
		},
	],
	fetch: async (data, cache) => {

		let results = {};

		// Sort the Oids by ttl
		const sortedOids = data["snmp.array"].reduce((obj, item) => {
			const ttl = item.ttl || 2000;
			obj[ttl] = obj[ttl] || [];
			obj[ttl].push(Snmp.normalizeOid(item.oid));
			return obj;
		}, {});

		for (const ttl in sortedOids) {
			const oids = sortedOids[ttl];
			const localResults = await cache.get(
				"snmp.oid",
				data["snmp.host"],
				data["snmp.community"],
				oids,
				ttl
			);
			Object.assign(results, localResults);
		}

		console.log(results);
		return results;
	},
};
