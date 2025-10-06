import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";

const Exception = ExceptionFactory("services", "provider");
const Log = LogFactory("services", "provider");

/// Trigger for event triggered processes.
class Trigger {
	constructor() {
		this.callback = null;
	}

	register(callback) {
		Exception.assert(this.callback === null, "Trigger was already registered.");
		this.callback = callback;
	}

	unregister() {
		Exception.assert(this.callback !== null, "Trigger was not registered.");
		this.callback = null;
	}

	trigger() {
		if (this.callback !== null) {
			this.callback();
		}
	}
}

/// A service is an entity that has a start, stop and one or more time triggered methods.
///
/// The start and stop are guaranteed to be called before the time triggered methods.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.prefix = "";
		this.processesStart = new Set();
		this.processesStop = new Set();
		this.processes = {};
	}

	/// Create a nested provider that maps to its parent.
	makeNested(...namespace) {
		Exception.assert(namespace.length > 0, "Nested services must have a namespace");
		const provider = new Provider(...this.namespace);
		provider.prefix = namespace.join(".") + ".";
		provider.processesStart = this.processesStart;
		provider.processesStop = this.processesStop;
		provider.processes = this.processes;
		return provider;
	}

	_makeName(name) {
		return this.prefix ? this.prefix + name : name;
	}

	_addProcess(name, process, options, trigger = null) {
		Exception.assert(
			!(name in this.processes),
			"A '{}' process is already registered for '{}'.",
			name,
			this.namespace.join("."),
		);
		this.processes[name] = {
			process: process,
			options: options,
			trigger: trigger,
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
	*getProcesses() {
		for (const [name, object] of Object.entries(this.processes)) {
			if (!this.processesStart.has(name) && !this.processesStop.has(name)) {
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
		return Object.assign(this.commongOptions, {
			policy: Services.Policy.throw,
		});
	}

	addStartProcess(name, process, options = {}) {
		const actualName = this._makeName(name);
		this.processesStart.add(actualName);
		this._addProcess(actualName, process, Object.assign(this.commongStartOptions, options));
	}

	addStopProcess(name, process, options = {}) {
		const actualName = this._makeName(name);
		this.processesStop.add(actualName);
		this._addProcess(actualName, process, Object.assign(this.commongStopOptions, options));
	}

	addTimeTriggeredProcess(name, process, options = {}) {
		const actualName = this._makeName(name);
		this._addProcess(
			actualName,
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

	addEventTriggeredProcess(name, process, options = {}) {
		const actualName = this._makeName(name);
		const trigger = new Trigger();
		this._addProcess(actualName, process, Object.assign(this.commongOptions, {}, options), trigger);
		return trigger;
	}
}
