/**
 * Set temporary local storage
 */
export default class LocalStorage {
  static set(key, value) { window.localStorage.setItem(key, value); }

  static get(key, defaultValue) {
	const value = window.localStorage.getItem(key);
	return value === null ? defaultValue || null : value;
  }

  static remove(key) { window.localStorage.removeItem(key); }

  static removeAll() { window.localStorage.clear(); }
}
