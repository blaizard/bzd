/// Base class to describe a process.
///
/// A process is a statefull class that perform an action.
export default class Process {
	constructor() {
		if (typeof this.process !== "function") {
			throw new Error("A process must implement a 'process' function.");
		}
	}
}
