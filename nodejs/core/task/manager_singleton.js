"use strict";

import Manager from "./manager.js";
import LogFactory from "../log.js";
import ExceptionFactory from "../exception.js";

const Log = LogFactory("task", "manager", "singleton");
const Exception = ExceptionFactory("task", "manager", "singleton");

// Create a unique, global symbol name
const TaskManager = Symbol.for("bzd.nodejs.core.task.manager");

// Check if the global object has this symbol.
// Add it if it does not have the symbol, yet.
const globalSymbols = Object.getOwnPropertySymbols(global);
if (globalSymbols.indexOf(TaskManager) === -1) {
	global[TaskManager] = new Manager();
}

let manager = global[TaskManager];
export default {
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
