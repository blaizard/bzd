/**
 * Set temporary local storage
 */
export default class LocalStorage {
	static set(key, value) {
		if (window && window.localStorage) {
			window.localStorage.setItem(key, value);
		}
	}

	static get(key, defaultValue) {
		if (window && window.localStorage) {
			const value = window.localStorage.getItem(key);
			return (value === null) ? (defaultValue || null) : value;
		}
		return defaultValue || null;
	}

	static remove(key) {
		if (window && window.localStorage) {
			window.localStorage.removeItem(key);
		}
	}

	static removeAll() {
		if (window && window.localStorage) {
			window.localStorage.clear();
		}
	}
};
