/// Set persistent local storage
export default class LocalStorage {
	static set(key, value) {
		window.localStorage.setItem(key, value);
	}

	static setSerializable(key, value) {
		LocalStorage.set(key, JSON.stringify(value));
	}

	static get(key, defaultValue) {
		const value = window.localStorage.getItem(key);
		return value === null ? defaultValue || null : value;
	}

	static getSerializable(key, defaultValue) {
		const noData = Symbol();
		const serialized = LocalStorage.get(key, noData);
		if (serialized === noData) {
			return defaultValue;
		}
		return JSON.parse(serialized);
	}

	static remove(key) {
		window.localStorage.removeItem(key);
	}

	static removeAll() {
		window.localStorage.clear();
	}
}
