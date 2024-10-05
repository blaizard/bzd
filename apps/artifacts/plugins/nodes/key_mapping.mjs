/// Set of utility to manage dictionary keys from array of string.
export default {
	/// Hash a array of strings.
	keyToInternal(...internalsOrKeys) {
		return internalsOrKeys.join("\x01");
	},

	/// Unhash a array of strings.
	internalToKey(internal) {
		return internal.split("\x01");
	},

	/// Check if a hash starts with a specific key.
	internalStartsWithKey(internal, key) {
		const hash = key.join("\x01");
		if (internal.startsWith(hash)) {
			if (internal.length == hash.length) {
				return true;
			}
			if (internal[hash.length] == "\x01") {
				return true;
			}
		}
		return false;
	},
};
