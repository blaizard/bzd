/// Way to provide statistics information.
export default class Provider {
	constructor(...namespace) {
		this.namespace = namespace;
		this.calls = {};
		this.sizes = {};
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
}
