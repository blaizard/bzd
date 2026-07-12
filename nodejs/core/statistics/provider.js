import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("statistics");

/// Processor for a rate.
class ProcessorRates {
	constructor(data) {
		this.current = 0;
		this.rateMean = 0;
		this.rateMax = 0;
		this.rateCounter = 0;
	}

	update(value) {
		this.current += value;
	}

	process(durationS, provider) {
		const rate = this.current / durationS;
		const durationSMax60 = Math.min(60, durationS);
		const rateMean = ((60 - durationSMax60) * this.rateMean + durationSMax60 * rate) / 60;
		this.rateMax = Math.max(this.rateMax, rate);
		this.rateMean = rateMean;
		this.rateCounter += this.current;
		provider.set("rate", rate);
		provider.set("rate.mean", this.rateMean);
		provider.set("rate.max", this.rateMax);
		provider.set("rate.counter", this.rateCounter);

		this.current = 0;
	}
}

class DataProxy {
	static uid = 0;

	constructor() {
		this.data = null;
		this.namespace = [];
		this.processors = {};
		this.uid = DataProxy.uid++;
	}

	register(data, processors, ...namespace) {
		this.data = data;
		this.processors = processors;
		this.namespace = namespace;
	}

	insert(...args) {
		if (this.data) {
			this.data.insert(...args);
		}
	}

	processor(key, create) {
		const path = ["data-proxy", this.uid, ...key].join(".");
		if (!(path in this.processors)) {
			this.processors[path] = create();
		}
		return this.processors[path];
	}
}

/// Way to provide statistics information.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.proxy = new DataProxy();
		this.previous = {};
	}

	/// Create a nested statistics provider.
	makeNested(...namespace) {
		Exception.assert(namespace.length > 0, "Nested statistics must have a namespace");
		const provider = new Provider(...this.namespace, ...namespace);
		provider.proxy = this.proxy;
		return provider;
	}

	_processor(key, create) {
		const [_, processor] = this.proxy.processor(key, () => [this.makeNested(...key), create()]);
		return processor;
	}

	_insert(key, value) {
		this.proxy.insert("statistics", [[[...this.proxy.namespace, ...this.namespace, ...key], value]]);
	}

	_insertAbsolute(key, value) {
		this.proxy.insert("statistics", [[[...key], value]]);
	}

	/// Initialize the data structure of a data point.
	_initData(name, initial) {
		return {
			[name]: initial,
			max: initial,
			min: initial,
			avg: initial,
			count: 0,
		};
	}

	/// Update the statistics of the data point.
	_updateData(name, data, value) {
		++data.count;
		data[name] = value;
		data.max = Math.max(value, data.max);
		data.min = Math.min(value, data.min);
		data.avg = (data.avg * (data.count - 1) + value) / data.count;
	}

	/// Time the given process.
	async timeit(name, callback) {
		const start = performance.now();
		try {
			await callback();
		} finally {
			this._insert([name, "duration"], (performance.now() - start) / 1000);
		}
	}

	/// Set a value point to the existing points.
	set(name, value) {
		this._insert([name], value);
	}

	/// Set a size point to the existing points.
	size(name, value) {
		this._insert([name, "size"], value);
	}

	/// Add a value point to the existing points.
	sum(name, value, initial = 0) {
		this.previous[name] ??= initial;
		this.previous[name] += value;
		this._insert([name, "sum"], this.previous[name]);
	}

	/// Calculate the rate of a point.
	///
	/// Compute the rate per/seconds.
	rate(name, value = 1) {
		this._processor([name], () => new ProcessorRates()).update(value);
	}
}
