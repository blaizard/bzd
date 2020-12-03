import SnmpNative from "net-snmp";
import ExceptionFactory from "bzd/core/exception.mjs";

const Exception = ExceptionFactory("snmp");

class Snmp {
	constructor(host, community) {
		this.session = new SnmpNative.createSession(host, community, {
			version: SnmpNative.Version2c,
		});
	}

	_castValue(data) {
		switch (data.type) {
		case SnmpNative.ObjectType.OctetString:
			return data.value.toString();
		}
		Exception.unreachable("Unsupported value type.");
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
						result[varbinds[i].oid] = this._castValue(varbinds[i]);
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
			fetch: async (host, community, oid, ttl, previous, options) => {
				// Update the time in ms.
				options.timeout = ttl * 1000;

				// Get the data
				const snmp = new Snmp(host, community);
				const result = await snmp.getAndClose([oid]);
				return { value: result[Object.keys(result)[0]] };
			},
			timeout: 2 * 1000,
		},
	],
	fetch: async (data, cache) => {
		console.log(data);
		const results = await cache.get(
			"snmp.oid",
			data["snmp.host"],
			data["snmp.community"],
			"1.3.6.1.4.1.6574.1.5.1.0",
			60 * 1000
		);
		console.log(results);
		return results;
	},
};
