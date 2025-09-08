import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("statistics");

/// Way to provide statistics information.
export default class Provider {
	constructor(...namespace) {
		this.root = {};
		this.data = Provider._rootToData(this.root, ...namespace);
	}

	static _rootToData(root, ...namespace) {
		let data = root;
		for (const name of namespace) {
			data[name] ??= {};
			data = data[name];
		}
		return data;
	}

	/// Create a nested statistics provider.
	makeNested(...namespace) {
		Exception.assert(namespace.length > 0, "Nested statistics must have a namespace");
		const statistics = new Provider();
		statistics.root = this.root;
		statistics.data = Provider._rootToData(this.data, ...namespace);
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
		const start = Provider._getTimestamp();
		try {
			await callback();
		} finally {
			this.data[name] ??= {
				count: 0,
				duration: 0,
			};
			++this.data[name].count;
			this.data[name].duration = Provider._getTimestamp() - start;
		}
	}

	/// Set a value point to the existing points.
	set(name, value) {
		this.data[name] ??= this._initData(0);
		this._updateData(this.data[name], value);
	}

	/// Add a value point to the existing points.
	sum(name, value, initial = 0) {
		this.data[name] ??= this._initData(initial);
		this._updateData(this.data[name], this.data[name].value + value);
	}
}
