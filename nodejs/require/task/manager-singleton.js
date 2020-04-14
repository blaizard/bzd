"use strict";

const Log = require("../log.js")("task", "manager", "singleton");
const Exception = require("../exception.js")("task", "manager", "singleton");
const Manager = require("./manager.js");

// Create a unique, global symbol name
const TaskManager = Symbol.for("nodejs-lib.task.manager");

// Check if the global object has this symbol.
// Add it if it does not have the symbol, yet.
const globalSymbols = Object.getOwnPropertySymbols(global);
if (globalSymbols.indexOf(TaskManager) === -1) {
	global[TaskManager] = new Manager();
}

let manager = global[TaskManager];
module.exports = {
	register(namespace, name, action, config) {
		return manager.register(namespace, name, action, config);
	},
	unregister(taskId) {
		return manager.unregister(taskId);
	},
	toJson(namespace) {
		return manager.toJson(namespace);
	}
};
