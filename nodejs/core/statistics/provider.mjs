import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("statistics");

/// Processor for a rate.
class ProcessorRates {
	constructor(data) {
		this.data = Object.assign(data, {
			rate: 0,
			rateAvg: 0,
			count: 0,
			max: 0,
		});
		this.current = 0;
	}

	update(value) {
		this.current += value;
	}

	process(durationS) {
		const rate = this.current / durationS;
		const durationSMax60 = Math.min(60, durationS);
		Object.assign(this.data, {
			rate: rate,
			rateAvg: ((60 - durationSMax60) * this.data.rateAvg + durationSMax60 * rate) / 60,
			count: this.data.count + this.current,
			max: Math.max(rate, this.data.max),
		});
		this.current = 0;
	}
}

/// Way to provide statistics information.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.namespaceStr = namespace.join(".");
		this.root = {};
		this.data = Provider._rootToData(this.root, ...namespace);
		this.processors = {};
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
		const statistics = new Provider(...this.namespace, ...namespace);
		statistics.root = this.root;
		statistics.processors = this.processors;
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

	/// Calculate the rate of a point.
	///
	/// Compute the rate per/seconds.
	rate(name, value = 1) {
		const namespace = this.namespaceStr + "." + name;
		this.data[name] ??= {};
		this.processors[namespace] ??= new ProcessorRates(this.data[name]);
		this.processors[namespace].update(value);
	}
}
