import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Services from "#bzd/apps/accounts/backend/services/services.mjs";

const Exception = ExceptionFactory("services", "provider");
const Log = LogFactory("services", "provider");

/// A service is an entity that has a start, stop and one or more time triggered methods.
///
/// The start and stop are guaranteed to be called before the time triggered methods.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.processes = {};
	}

	_addProcess(name, process, options) {
		Exception.assert(
			!(name in this.processes),
			"A '{}' process is already registered for '{}'.",
			name,
			this.namespace.join("."),
		);
		this.processes[name] = {
			process: process,
			options: options,
		};
	}

	/// Helpers to return certain types of processes.
	*getStartProcesses() {
		if ("start" in this.processes) {
			yield ["start", this.processes["start"]];
		}
	}
	*getStopProcesses() {
		if ("stop" in this.processes) {
			yield ["stop", this.processes["stop"]];
		}
	}
	*getTimeTriggeredProcesses() {
		for (const [name, object] of Object.entries(this.processes)) {
			if (!["start", "stop"].includes(name)) {
				yield [name, object];
			}
		}
	}

	get commongOptions() {
		return {
			/// Maximum number of logs to be kept.
			maxLogs: 10,
			/// The policy to be used when the process stops with an error.
			policy: Services.Policy.ignore,
		};
	}

	addStartProcess(process, options = {}) {
		this._addProcess("start", process, Object.assign(this.commongOptions, options));
	}

	addStopProcess(process, options = {}) {
		this._addProcess("stop", process, Object.assign(this.commongOptions, options));
	}

	addTimeTriggeredProcess(name, process, options = {}) {
		Exception.assert(!["start", "stop"].includes(name), "The process name '{}' cannot be used.", name);
		this._addProcess(
			name,
			process,
			Object.assign(
				this.commongOptions,
				{
					/// Period in secoonds at which this service should run.
					periodS: 24 * 3600,
					/// The first start will be after a certain number of seconds.
					/// If unset, the amount will be random within the periodS given.
					delayS: null,
				},
				options,
			),
		);
	}
}
