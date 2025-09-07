import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("statistics");

/// Way to provide statistics information.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.namespaceStr = namespace.length ? namespace.join(".") + "." : "";
		this.calls = {};
		this.data = {};
	}

	/// Create a nested statistics provider.
	makeNested(...namespace) {
		Exception.assert(namespace.length > 0, "Nested statistics must have a namespace");
		const statistics = new Provider(...this.namespace, ...namespace);
		statistics.calls = this.calls;
		statistics.data = this.data;
		return statistics;
	}

	/// Get the current timestamp in ms.
	static _getTimestamp() {
		return Date.now();
	}

	/// Initialize the data structure of a data point.
	_initData(initial) {
		return {
			value: initial,
			max: initial,
			min: initial,
			avg: initial,
			count: 0,
		};
	}

	/// Update the statistics of the data point.
	_updateData(data, value) {
		++data.count;
		data.value = value;
		data.max = Math.max(value, data.max);
		data.min = Math.min(value, data.min);
		data.avg = (data.avg * (data.count - 1) + value) / data.count;
	}

	/// Time the given process.
	async timeit(name, callback) {
		const fqn = this.namespaceStr + name;
		const start = Provider._getTimestamp();
		try {
			await callback();
		} finally {
			this.calls[fqn] ??= {
				count: 0,
				duration: 0,
			};
			++this.calls[fqn].count;
			this.calls[fqn].duration = Provider._getTimestamp() - start;
		}
	}

	/// Set a value point to the existing points.
	set(name, value) {
		const fqn = this.namespaceStr + name;
		this.data[fqn] ??= this._initData(0);
		this._updateData(this.data[fqn], value);
	}

	/// Add a value point to the existing points.
	sum(name, value, initial = 0) {
		const fqn = this.namespaceStr + name;
		this.data[fqn] ??= this._initData(initial);
		this._updateData(this.data[fqn], this.data[fqn].value + value);
	}
}
