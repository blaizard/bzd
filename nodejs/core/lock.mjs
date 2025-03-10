/// Simple async lock implementation
export default class Lock {
	constructor() {
		this.lastTask = Promise.resolve(); // Starts as a resolved promise
	}

	acquire(callback) {
		// Chain the callback to the last task
		const nextTask = this.lastTask.then(callback).finally(() => {
			if (this.lastTask === nextTask) {
				this.lastTask = Promise.resolve(); // Reset when no tasks are pending
			}
		});

		this.lastTask = nextTask;
		return nextTask;
	}
}
