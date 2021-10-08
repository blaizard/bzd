import Manager from "./manager.mjs";

// Create a unique, global symbol name
const _TaskManager = Symbol.for("bzd.core.task.manager");

/*
 * Check if the global object has this symbol.
 * Add it if it does not have the symbol, yet.
 */
const globalSymbols = Object.getOwnPropertySymbols(global);
if (globalSymbols.indexOf(_TaskManager) === -1) {
	global[_TaskManager] = new Manager();
}

let manager = global[_TaskManager];
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
