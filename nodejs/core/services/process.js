/// Base class to describe a process.
///
/// A process is a stateful class that performs an action.
export default class Process {
	constructor() {
		if (typeof this.process !== "function") {
			throw new Error("A process must implement a 'process' function.");
		}
	}
}
