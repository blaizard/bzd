export default class Utils {
	/// Convert a URL path to a storage key (array of strings).
	static pathToKey(path) {
		return path.split("/").filter(Boolean).map(decodeURIComponent);
	}

	/// Convert a storage key (array of strings) to a URL path.
	static keyToPath(key) {
		return "/" + key.map(encodeURIComponent).join("/");
	}

	/// Get the current UTC timestamp.
	static timestampMs() {
		return Date.now();
	}
}
