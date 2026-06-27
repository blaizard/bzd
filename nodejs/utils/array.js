/// Find the common prefixes of an array.
export function arrayFindCommonPrefix(...arrays) {
	const first = arrays[0] ?? [];
	let commonPrefix = first.length;
	arrays.forEach((array, arrayIndex) => {
		if (arrayIndex) {
			let index = 0;
			for (; index < Math.min(array.length, first.length); ++index) {
				if (first[index] != array[index]) {
					break;
				}
			}
			commonPrefix = Math.min(commonPrefix, index);
		}
	});

	return first.slice(0, commonPrefix);
}
