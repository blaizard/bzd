import ExceptionFactory from "bzd/core/exception.mjs";
import LogFactory from "bzd/core/log.mjs";

const Log = LogFactory("services");
const Exception = ExceptionFactory("services");

export default class Services {
	constructor(options) {
		this.options = Object.assign(
			{
				/// Generate context based on the current name.
				/// Contet are passed to the start function.
				makeContext: (/*name*/) => ({}),
			},
			options
		);
		this.services = {};
		this.instances = {};
	}

	register(namespace, type, object) {
		Log.info("Registering service {}::{}", namespace, type);
		this.services[namespace] = this.services[namespace] || {};
		this.services[namespace][type] = object;
	}

	*_iterate(namespace) {
		for (const type in this.services[namespace]) {
			yield [type, this.services[namespace][type]];
		}
	}

	/// Get list of active services
	getActive() {
		let services = {};
		for (const name in this.instances) {
			services[name] = Object.keys(this.instances[name]);
		}
		return services;
	}

	async start(name, namespace, params) {
		Exception.assert(!(name in this.instances), "Instances for '{}' are already created.", name);
		for (const [type, service] of this._iterate(namespace)) {
			if (await service.is(params)) {
				Log.info("Starting service {}::{} for '{}'", namespace, type, name);
				try {
					const result = await service.start(params, this.options.makeContext(name));
					this.instances[name] = this.instances[name] || {};
					this.instances[name][type] = {
						result: result,
						service: service,
					};
				} catch (e) {
					Exception.print("{}", Exception.fromError(e));
				}
			}
		}
	}

	async stop(name) {
		try {
			for (const type in this.instances[name] || {}) {
				const data = this.instances[name][type];
				Log.info("Stopping service '{}' for '{}'", type, name);
				try {
					await data.service.stop(data.result);
				} catch (e) {
					Exception.print("{}", Exception.fromError(e));
				}
			}
		} finally {
			delete this.instances[name];
		}
	}
}
