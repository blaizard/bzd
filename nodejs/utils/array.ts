/// Find the longest common prefix shared by all the given arrays.
export function arrayFindCommonPrefix<T>(...arrays: T[][]): T[] {
    const first = arrays[0] ?? [];
    let commonPrefix = first.length;

    arrays.forEach((array, arrayIndex) => {
        if (arrayIndex) {
            let index = 0;
            for (; index < Math.min(array.length, first.length); ++index) {
                if (first[index] !== array[index]) {
                    break;
                }
            }
            commonPrefix = Math.min(commonPrefix, index);
        }
    });

    return first.slice(0, commonPrefix);
}
