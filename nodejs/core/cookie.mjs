export default class Cookie {
	/// Read a cookie value by key value.
	static get(key, defaultValue) {
		// Loop through all the cookies
		for (const cookie of document.cookie.split(";")) {
			const [name, value] = cookie.split("=");
			if (key == name.trim()) {
				return decodeURIComponent(value.trim());
			}
		}
		return defaultValue;
	}

	/// Set a specific cookie value.
	static set(key, value, options = {}) {
		document.cookie = [
			key.trim() + "=" + encodeURIComponent(value),
			...Object.entries(options).map(([k, v]) => k + "=" + v),
		].join("; ");
	}

	/// Remove a specific cookie.
	static remove(key) {
		Cookie.set(key, "", {
			expires: "Thu, 01 Jan 1970 00:00:00 UTC",
		});
	}

	/// Remove all existing cookie.
	static removeAll() {
		for (const cookie of document.cookie.split(";")) {
			const [name, _] = cookie.split("=");
			Cookie.remove(name);
		}
	}
}
