class Action {
  constructor(data) { this.data = data; }

  /// The action type.
  get type() { return this.data.action; }

  /// The arguments associated with the action.
  get args() { return this.data.args || []; }

  toString() { return "\x1b[1;33m" + this.type + "\x1b[0;1m\n" + this.args + "\x1b[0m "; }
}

/// Handle a scenario
export default class Scenario {
  constructor(data = {}) { this.data = data; }

  get name() { return this.data.name || "project"; }

  /// The number of actions in the flow.
  get size() { return (this.data.flow || []).length; }

  /// Get the action at a specific index.
  at(index) {
	const data = (this.data.flow || [])[index];
	return data === undefined ? null : new Action(data);
  }
}
