/// Set temporary local storage
export default class SessionStorage {
	static set(key, value) {
		window.sessionStorage.setItem(key, value);
	}

	static setSerializable(key, value) {
		SessionStorage.set(key, JSON.stringify(value));
	}

	static get(key, defaultValue) {
		const value = window.sessionStorage.getItem(key);
		return value === null ? defaultValue || null : value;
	}

	static getSerializable(key, defaultValue) {
		const noData = Symbol();
		const serialized = SessionStorage.get(key, noData);
		if (serialized === noData) {
			return defaultValue;
		}
		return JSON.parse(serialized);
	}

	static remove(key) {
		window.sessionStorage.removeItem(key);
	}

	static removeAll() {
		window.sessionStorage.clear();
	}
}
