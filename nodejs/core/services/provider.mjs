import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";

const Exception = ExceptionFactory("services", "provider");
const Log = LogFactory("services", "provider");

/// A service is an entity that has a start, stop and one or more time triggered methods.
///
/// The start and stop are guaranteed to be called before the time triggered methods.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.processesStart = [];
		this.processesStop = [];
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
		for (const name of this.processesStart) {
			yield [name, this.processes[name]];
		}
	}
	*getStopProcesses() {
		for (const name of this.processesStop) {
			yield [name, this.processes[name]];
		}
	}
	*getTimeTriggeredProcesses() {
		for (const [name, object] of Object.entries(this.processes)) {
			if (!this.processesStart.includes(name) && !this.processesStop.includes(name)) {
				yield [name, object];
			}
		}
	}

	get commongOptions() {
		return {
			/// Maximum number of logs to be kept.
			maxLogs: 10,
			/// Maximum of error logs to be kept.
			maxErrorLogs: 5,
			/// The policy to be used when the process stops with an error.
			policy: Services.Policy.ignore,
		};
	}

	get commongStartOptions() {
		return Object.assign(this.commongOptions, {
			policy: Services.Policy.throw,
		});
	}

	get commongStopOptions() {
		return this.commongOptions;
	}

	addStartProcess(process, options = {}) {
		const name = "start." + (this.processesStart.length + 1);
		this.processesStart.push(name);
		this._addProcess(name, process, Object.assign(this.commongStartOptions, options));
	}

	addStopProcess(process, options = {}) {
		const name = "stop." + (this.processesStop.length + 1);
		this.processesStop.push(name);
		this._addProcess(name, process, Object.assign(this.commongStopOptions, options));
	}

	addTimeTriggeredProcess(name, process, options = {}) {
		Exception.assert(
			!name.startsWith("start.") && !name.startsWith("stop."),
			"The process name '{}' cannot be used.",
			name,
		);
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
