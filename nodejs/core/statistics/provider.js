import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("statistics");

/// Processor for computing rate statistics (current, mean, max).
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
		const kSmoothingWindowS = 60;
		const durationSMax = Math.min(kSmoothingWindowS, durationS);
		const rateMean = ((kSmoothingWindowS - durationSMax) * this.rateMean + durationSMax * rate) / kSmoothingWindowS;
		this.rateMax = Math.max(this.rateMax, rate);
		this.rateMean = rateMean;
		this.rateCounter += this.current;
		provider.set("rate", rate, { unit: "t/s" });

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

class DataInserter {
	constructor(provider) {
		this.provider = provider;
		this.timestamp = null;
	}

	_processor(key, create) {
		const [_, processor] = this.provider.proxy.processor(key, () => [this.provider.makeNested(...key), create()]);
		return processor;
	}

	set(key, value, options) {
		const actualKey = Array.isArray(key) ? key : [...this.provider.proxy.namespace, ...this.provider.namespace, key];
		this.timestamp = this.provider.proxy.insert("statistics", [[actualKey, value, options ?? {}]], this.timestamp);
		return this;
	}

	size(key, value) {
		return this.set(key, value, { unit: "By" });
	}

	sum(key, value, options = 0) {
		this.provider.previous[key] ??= 0;
		this.provider.previous[key] += value;
		return this.set(key, this.provider.previous[key], options);
	}

	rate(name, value = 1) {
		this._processor([name], () => new ProcessorRates()).update(value);
		return this;
	}

	async timeit(key, callback) {
		const start = performance.now();
		try {
			await callback();
		} finally {
			this.set(key, (performance.now() - start) / 1000, { unit: "s" });
		}
		return this;
	}
}

/// Provider for collecting and reporting statistics data.
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

	/// Record a value data point.
	set(key, value, options) {
		return new DataInserter(this).set(key, value, options);
	}

	/// Record a size data point (in bytes).
	size(key, value) {
		return new DataInserter(this).size(key, value);
	}

	/// Accumulate a value across successive calls.
	sum(key, value, options) {
		return new DataInserter(this).sum(key, value, options);
	}

	/// Track an event rate (events per second).
	rate(key, value = 1) {
		return new DataInserter(this).rate(key, value);
	}

	/// Time the given process.
	async timeit(key, callback) {
		return new DataInserter(this).timeit(key, callback);
	}
}
