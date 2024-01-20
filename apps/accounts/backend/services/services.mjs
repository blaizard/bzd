import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("services");
const Log = LogFactory("services");

/// This class is a manager of service, that start, stop and schedule them.
///
/// A service is an entity that has a start, stop and one or more time triggered methods.
export default class Services {
	/// Status of a service.
	/// Emulation of an enum type.
	static Status = Object.freeze({
		idle: "idle",
		starting: "starting",
		running: "running",
		stoping: "stoping",
		error: "error",
	});

	/// Type of polcies, what action is triggered in case of error.
	/// Emulation of an enum type.
	static Policy = Object.freeze({
		/// No operation.
		ignore: "ignore",
		/// Restart the whole
		restart: "restart",
	});

	constructor() {
		this.services = {};
	}

	/// Get the current timestamp in ms.
	static _getTimestamp() {
		return Date.now();
	}

	/// Create the service UID from the name and the service provider.
	static _makeServiceUid(name, serviceProvider) {
		return serviceProvider.namespace.join(".") + "." + name;
	}

	/// Run a process and return a log.
	static async _runProcess(uid, name, process) {
		const timestampStart = Services._getTimestamp();
		let error = null;
		try {
			Log.info("Running '{}.{}'...", uid, name);
			await process();
			Log.info("Completed '{}.{}'.", uid, name);
		} catch (e) {
			Log.error("Error '{}.{}': {}.", uid, name, e);
			error = e;
		}

		return {
			error: error,
			timestampStart: timestampStart,
			timestampStop: Services._getTimestamp(),
		};
	}

	static _saveProcessRecord(service, name, processResult) {
		/// Check the maximum number of logs for this enty.
		const maxLogs = service.provider.processes[name].options.maxLogs;
		Exception.assert(typeof maxLogs == "number", "Maxlogs must be a number, instead: {}.", maxLogs);

		service.records[name] ??= {
			executions: 0,
			errors: 0,
			logs: [],
		};

		++service.records[name].executions;
		if (processResult.error) {
			++service.records[name].errors;
		}
		service.records[name].logs.unshift(processResult);
		if (service.records[name].logs.length > maxLogs) {
			service.records[name].logs.splice(maxLogs);
		}
	}

	/// Run a certain process and save the logs.
	///
	/// \param uid The uid corresponding to the service.
	/// \param name The name of the process.
	/// \return true if succeed, false otherwise.
	async runProcess(uid, name) {
		const object = this.services[uid].provider.processes[name];
		const processResult = await Services._runProcess(uid, name, object.process);
		Services._saveProcessRecord(this.services[uid], name, processResult);
		return processResult.error === null;
	}

	/// Register a service.
	///
	/// \param name The name of the service.
	/// \param provider The service provider for this service.
	/// \return The UID of the service.
	register(name, provider) {
		const uid = Services._makeServiceUid(name, provider);
		Exception.assert(!(uid in this.services), "Service '{}' is already registered.", uid);

		this.services[uid] = {
			provider: provider,
			instances: {},
			// {
			//	 <name>: {
			//      executions: 0,
			//      errors: 0,
			//      logs: [],
			//   }
			// }
			records: {},
			state: {
				status: Services.Status.idle,
				/// Timestamp of the last start.
				timestampStart: 0,
				/// Timestamp of the last stop.
				timestampStop: 0,
			},
		};

		Log.info("Registering service '{}'.", uid);
		return uid;
	}

	/// Start a specific service.
	async startService(uid) {
		const service = this.services[uid];
		Exception.assert(
			service.state.status === Services.Status.idle,
			"Service '{}' already started, status: '{}'.",
			uid,
			service.state.status,
		);
		Exception.assert(
			!service.instances.length,
			"Some processes from '{}' are already started: {}.",
			uid,
			Object.keys(service.instances),
		);

		service.state.timestampStart = Services._getTimestamp();
		service.state.status = Services.Status.starting;

		// Start the start processes if any.
		for (const [name, _] of service.provider.getStartProcesses()) {
			if (!(await this.runProcess(uid, name))) {
				service.state.timestampStop = Services._getTimestamp();
				service.state.status = Services.Status.error;
				return false;
			}
		}

		// Start the time triggered processes if any.
		for (const [name, object] of service.provider.getTimeTriggeredProcesses()) {
			const delayS =
				object.options.delayS === null ? Math.floor(Math.random() * object.options.periodS) : object.options.delayS;
			Log.info("Starting '{}.{}' every {}s with a delay of {}s.", uid, name, object.options.periodS, delayS);

			service.instances[name] = setTimeout(async () => {
				if (!(await this.runProcess(uid, name))) {
					switch (object.options.policy) {
						case Services.Policy.ignore:
							break;
						case Services.Policy.restart:
							break;
					}
				}
			}, delayS * 1000);
		}

		service.state.status = Services.Status.running;
	}

	async start() {
		for (const uid of Object.keys(this.services)) {
			await this.startService(uid);
		}
	}

	/// List all the services available and their states.
	///
	/// \return A tuple of the UID and the state.
	*getServices() {
		for (const [uid, service] of Object.entries(this.services)) {
			yield [uid, service.state];
		}
	}

	/// Get a specific service given it's UID.
	///
	/// \return The service state.
	getService(uid) {
		Exception.assert(uid in this.services, "The service '{}' is not registered.", uid);
		return this.services[uid].state;
	}

	async installAPI(api) {
		Log.info("Installing 'Services' API");

		// ---- Admin specific API

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
