/// Set of utility to manage dictionary keys from array of string.
const KeyMapping = {
	/// Hash a array of strings.
	keyToInternal(internalsOrKeys) {
		return internalsOrKeys.join("\x01");
	},

	/// Unhash a array of strings.
	internalToKey(internal) {
		return internal.split("\x01");
	},

	/// Check if a hash starts with a specific key.
	keyStartsWith(key, root) {
		return key.slice(0, root.length).toString() == root.toString();
	},

	/// Convert /hello/you -> ["hello", "you"]
	pathToKey(path) {
		return path.split("/").filter(Boolean);
	},

	/// Convert /hello/you -> "hello\x01you"
	pathToInternal(path) {
		return KeyMapping.keyToInternal(KeyMapping.pathToKey(path));
	},

	/// Convert ["hello", "you"] -> /hello/you
	keyToPath(key) {
		return "/" + key.join("/");
	},

	/// Convert "hello\0x01you" -> /hello/you
	internalToPath(internal) {
		return KeyMapping.keyToPath(KeyMapping.internalToKey(internal));
	},
};

export default KeyMapping;
