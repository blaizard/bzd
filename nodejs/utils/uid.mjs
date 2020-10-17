/**
 * Generate a unique ID
 */
let _uidCounter = 0;
export function makeUid() {
	return "uid-" + Date.now() + "-" + Math.random().toString(36).substr(2, 8) + "-" + ++_uidCounter;
}
