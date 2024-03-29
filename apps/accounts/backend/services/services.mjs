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
		stopping: "stopping",
		error: "error",
	});

	/// Type of policies, what action is triggered in case of error.
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
	_makeServiceUid(name, provider) {
		if (name === null) {
			let counter = 0;
			do {
				name = (++counter).toString();
			} while (this._makeServiceUid(name, provider) in this.services);
		}
		return provider.namespace.join(".") + "." + name;
	}

	/// Run a certain process and save the logs.
	///
	/// \param uid The uid corresponding to the service.
	/// \param name The name of the process.
	/// \return true if succeed, false otherwise.
	async runProcess(uid, name) {
		const service = this.services[uid];
		const object = service.provider.processes[name];
		service.records[name] ??= {
			executions: 0,
			errors: 0,
			durationMin: 0,
			durationMax: 0,
			durationAvg: 0,
			status: Services.Status.idle,
			logs: [],
		};
		const record = service.records[name];

		let log = {
			error: null,
			timestampStart: Services._getTimestamp(),
			timestampStop: 0,
			result: null,
		};

		// Check the maximum number of logs for this enty.
		const maxLogs = object.options.maxLogs;
		Exception.assert(typeof maxLogs == "number", "Maxlogs must be a number, instead: {}.", maxLogs);

		// Insert the new log already.
		record.logs.unshift(log);
		if (record.logs.length > maxLogs) {
			record.logs.splice(maxLogs);
		}

		try {
			Log.debug("Running '{}.{}'...", uid, name);
			++record.executions;
			record.status = Services.Status.running;
			log.result = await object.process();
			record.status = Services.Status.idle;
			Log.debug("Completed '{}.{}'.", uid, name);
		} catch (e) {
			Log.error("Error '{}.{}': {}.", uid, name, e);
			++record.errors;
			record.status = Services.Status.error;
			log.error = e;
		} finally {
			log.timestampStop = Services._getTimestamp();
		}

		const duration = (log.timestampStop - log.timestampStart) / 1000;
		record.durationMin = Math.min(record.durationMin || duration, duration);
		record.durationMax = Math.max(record.durationMax, duration);
		record.durationAvg = (record.durationAvg * (record.executions - 1) + duration) / record.executions;

		return log.error === null;
	}

	/// Run a time triggered process.
	///
	/// \param uid The uid corresponding to the service.
	/// \param name The name of the process.
	/// \param periodS The recurrency period.
	async runTimeTriggeredProcess(uid, name, periodS) {
		const service = this.services[uid];
		const object = service.provider.processes[name];

		if (!(await this.runProcess(uid, name))) {
			switch (object.options.policy) {
				case Services.Policy.ignore:
					break;
				case Services.Policy.restart:
					break;
			}
		}

		// Re-run, if name is not present it means that the service was stopped.
		if (name in service.instances) {
			service.instances[name] = setTimeout(async () => {
				await this.runTimeTriggeredProcess(uid, name, periodS);
			}, periodS * 1000);
		}
	}

	/// Register a service.
	///
	/// \param provider The service provider for this service.
	/// \param name The name of the service.
	/// \return The UID of the service.
	register(provider, name = null) {
		const uid = this._makeServiceUid(name, provider);
		Exception.assert(!(uid in this.services), "Service '{}' is already registered.", uid);

		this.services[uid] = {
			provider: provider,
			instances: {},
			// {
			//	 <name>: {
			//      executions: 0,
			//      errors: 0,
			//      status: Services.Status.idle,
			//      durationMin: 0,
			//      durationMax: 0,
			//      durationAvg: 0,
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
		Log.info("Starting '{}'.", uid);

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
				await this.runTimeTriggeredProcess(uid, name, object.options.periodS);
			}, delayS * 1000);
		}

		service.state.status = Services.Status.running;
		return true;
	}

	/// Stop a specific service.
	async stopService(uid) {
		const service = this.services[uid];
		service.state.status = Services.Status.stopping;
		for (const [name, instance] of Object.entries(service.instances)) {
			clearTimeout(instance);
			Log.info("Stopped '{}.{}'.", uid, name);
		}
		service.instances = {};

		// Start the stop processes if any.
		for (const [name, _] of service.provider.getStopProcesses()) {
			await this.runProcess(uid, name);
		}

		service.state.timestampStop = Services._getTimestamp();
		service.state.status = Services.Status.idle;

		Log.info("Stopped '{}'.", uid);
	}

	/// Start all services.
	async start() {
		for (const uid of Object.keys(this.services)) {
			await this.startService(uid);
		}
	}

	/// Stop all services.
	async stop() {
		for (const uid of Object.keys(this.services)) {
			await this.stopService(uid);
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
	/// \param uid The service UID.
	/// \return The service state.
	getService(uid) {
		Exception.assertPrecondition(uid in this.services, "The service '{}' is not registered.", uid);
		return this.services[uid].state;
	}

	/// Get all the processes of a specific service.
	///
	/// \param uid The service UID.
	/// \return A tuple of process name and record.
	*getProcesses(uid) {
		Exception.assertPrecondition(uid in this.services, "The service '{}' is not registered.", uid);
		for (const [name, record] of Object.entries(this.services[uid].records)) {
			yield [name, record];
		}
	}

	/// Get a specific process record.
	///
	/// \param uid The service UID.
	/// \param name The process name.
	/// \return The process record.
	getProcess(uid, name) {
		Exception.assertPrecondition(uid in this.services, "The service '{}' is not registered.", uid);
		Exception.assertPrecondition(
			name in this.services[uid].records,
			"The process '{}' is part of service '{}'.",
			name,
			uid,
		);
		return this.services[uid].records[name];
	}

	/// Install services from all the objects passed into argument.
	async installServices(...objects) {
		for (const object of objects) {
			if (typeof object.installService == "function") {
				await object.installService(this);
			}
		}
	}

	async installRest(api) {
		Log.info("Installing 'Services' REST");

		// ---- Admin specific API

		/// Return the following:
		///
		/// services: {
		///     uid: {
		///	       state: { ... },
		///        processes: {
		///	          name: {
		///	             status: ...
		///              ...
		///           }
		///        }
		///     },
		///     ...
		/// }
		api.handle("get", "/admin/services", async () => {
			let services = {};
			for (const [uid, state] of this.getServices()) {
				let processes = {};
				for (const [name, record] of this.getProcesses(uid)) {
					processes[name] = Object.assign({}, record, {
						logs: null,
					});
				}
				services[uid] = {
					state: state,
					processes: processes,
				};
			}

			return {
				services: services,
				timestamp: Services._getTimestamp(),
			};
		});

		api.handle("get", "/admin/service/logs", async (inputs) => {
			const record = this.getProcess(inputs.uid, inputs.name);
			return {
				logs: record.logs,
			};
		});

		api.handle("post", "/admin/service/trigger", async (inputs) => {
			await this.runProcess(inputs.uid, inputs.name);
		});
	}
}
