import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("artifacts", "utils");

export default class Utils {
	/// Convert a URL path to a storage key (array of strings).
	static pathToKey(path) {
		Exception.assertPrecondition(typeof path === "string", "The path must be a string, not: {:?}", path);
		return path
			.split("/")
			.filter(Boolean)
			.map((segment) => {
				try {
					return decodeURIComponent(segment);
				} catch (e) {
					Exception.errorPrecondition("The path is malformed, the segment '{}' cannot be decoded.", segment);
				}
			});
	}

	/// Convert a storage key (array of strings) to a URL path.
	static keyToPath(key) {
		return "/" + key.map(encodeURIComponent).join("/");
	}

	/// Normalize a storage key by collapsing "." and ".." segments.
	///
	/// \throws If a ".." segment would expand beyond the root.
	static sanitizeKey(key) {
		const sanitized = [];
		for (const segment of key) {
			if (segment == "..") {
				const result = sanitized.pop();
				Exception.assertPrecondition(
					result !== undefined,
					"Key '{}' cannot be sanitized, '..' expands beyond the root.",
					key,
				);
			} else if (segment != ".") {
				sanitized.push(segment);
			}
		}
		return sanitized;
	}
}
