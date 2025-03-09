/// Simple async lock implementation
export default class Lock {
	constructor() {
		this.queue = [];
		this.isLock = false;
	}

	_next() {
		if (this.queue.length > 0) {
			const callback = this.queue.shift();
			// Immediately process the next item with the next tick. This is done in order
			// to avoid long call stacks.
			setTimeout(callback, 0);
		} else {
			this.isLock = false;
		}
	}

	/// Lock the resource for the duration of the callback.
	acquire(callback) {
		return new Promise((resolve, reject) => {
			const wrapper = () => {
				callback()
					.then(resolve)
					.catch(reject)
					.finally(() => {
						this._next();
					});
			};

			if (this.isLock) {
				this.queue.push(wrapper);
			} else {
				this.isLock = true;
				wrapper();
			}
		});
	}
}
