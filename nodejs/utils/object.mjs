/// Return the difference between object1 and object2.
export function objectDifference(object1, object2) {
	const keys = new Set([...Object.keys(object1), ...Object.keys(object2)]);
	let difference = {};
	for (const key of keys) {
		if (key in object1 && !(key in object2)) {
			difference[key] = object1[key];
		} else if (!(key in object1) && key in object2) {
			difference[key] = object2[key];
		}
		// The key is in both but they hhave different types
		else if (typeof object1[key] !== typeof object2[key]) {
			difference[key] = object2[key];
		}
		// The key is in both and points to objects
		else if (typeof object1[key] === "object" && !Array.isArray(object1[key]) && !Array.isArray(object2[key])) {
			difference[key] = objectDifference(object1[key], object2[key]);
		}
		// The key is in both and points to different values
		else if (object1[key] !== object2[key]) {
			difference[key] = object2[key];
		}
	}
	return difference;
}
