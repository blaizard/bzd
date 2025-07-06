/// Way to provide statistics information.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.calls = {};
		this.data = {};
	}

	/// Get the current timestamp in ms.
	static _getTimestamp() {
		return Date.now();
	}

	/// Time the given process.
	async timeit(name, callback) {
		const start = Provider._getTimestamp();
		try {
			await callback();
		} finally {
			this.calls[name] ??= {
				count: 0,
				duration: 0,
			};
			++this.calls[name].count;
			this.calls[name].duration = Provider._getTimestamp() - start;
		}
	}

	/// Register the given size.
	setSize(name, size) {
		this.sizes[name] ??= {
			current: size,
			max: size,
		};
		this.sizes[name].current = size;
		this.sizes[name].max = Math.max(size, this.sizes[name].max);
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
