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
			console.log("Getting " + oids);
			this.session.get(oids, (error, varbinds) => {
				console.log("Received " + oids);

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
			collection: "snmp.oids",
			fetch: async () => {
				const snmp = new Snmp("192.168.1.200", "public");
				return await snmp.getAndClose(["1.3.6.1.4.1.6574.1.5.1.0"]);
			},
			timeout: 2 * 1000,
		},
	],
	fetch: async (data, cache) => {
		console.log(data);
		const results = await cache.get("snmp.oids");
		console.log(results);
		return results;
	},
};
