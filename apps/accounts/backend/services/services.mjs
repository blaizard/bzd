import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import APISchema from "./api.json" assert { type: "json" };

const Exception = ExceptionFactory("services");
const Log = LogFactory("services");

export default class Services {
	constructor() {
		this.schema = {};
		this.metadata = {};
		this.instances = {};
	}

	register(type, generator, process, options = {}) {
		Exception.assert(!(type in this.schema), "Service type '{}' is already registered.", type);

		this.schema[type] = {
			generator: generator,
			process: process,
			options: Object.assign(
				{
					/// Period in s at which this service should run
					periodS: 24 * 3600,
				},
				options,
			),
		};

		this.metadata[type] = {
			status: "not started",
			lastErrors: 0,
			lastStarted: 0,
			lastDuration: 0,
			lastProcessed: 0,
		};

		Log.info("Registering service '{}'.", type);
	}

	start() {
		for (const type in this.schema) {
			Exception.assert(!(type in this.instances), "Service '{}' already started.", type);

			this.instances[type] = setTimeout(
				async () => {
					await this._process(type);
				},
				Math.random() * this.schema[type].options.periodS * 1000,
			);

			Log.info("Starting service '{}' every {}s", type, this.schema[type].options.periodS);
		}
	}

	async _process(type) {
		let metadata = this.metadata[type];
		metadata.status = "running";
		metadata.lastErrors = 0;
		metadata.lastProcessed = 0;
		metadata.lastStarted = Date.now();

		try {
			Log.info("Running service '{}'...", type);

			let it = await this.schema[type].generator();
			for await (const [name, data] of it) {
				try {
					if (await this.schema[type].process(name, data)) {
						++metadata.lastProcessed;
					}
				} catch (e) {
					Exception.fromError(e).print();
					++metadata.lastErrors;
				}
			}
		} catch (e) {
			Exception.fromError(e).print();
			++metadata.lastErrors;
		} finally {
			metadata.status = "stopped";
			metadata.lastDuration = Date.now() - metadata.lastStarted;
			Log.info("Service '{}' completed.", type);

			this.instances[type] = setTimeout(async () => {
				await this._process(type);
			}, this.schema[type].options.periodS * 1000);
		}
	}

	installAPI(api) {
		Log.info("Installing 'Services' API");

		// ---- Admin specific API

		api.addSchema(APISchema);

		api.handle("get", "/admin/services", async () => {
			return Object.keys(this.schema).reduce((obj, type) => {
				obj[type] = this.metadata[type];
				return obj;
			}, {});
		});

		api.handle("post", "/admin/service", async (inputs) => {
			if (inputs.uid) {
				await this.schema[inputs.type].process(inputs.uid);
			} else {
				this._process(inputs.type);
			}
		});
	}
}
