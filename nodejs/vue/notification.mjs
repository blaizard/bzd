

export default {
	install(Vue) {
		Vue.prototype.$notification = new Vue({
			data: {
				entries: [],
				_uid: 0
			},
			methods: {
				_tryToString(message) {
					if (message instanceof Error) {
						const splittedMessage = String(message).split("\n");
						message = splittedMessage[0] || "";
						if (splittedMessage.length > 1) {
							message += "...";
						}
					}
					return message;
				},
				_notify(type, message, options = {}) {
					message = this._tryToString(message);
					if (typeof message == "object") {
						for (const key in message) {
							this._notifySingle(key, type, message[key], options);
						}
					}
					else {
						this._notifySingle(++this._uid, type, message, options);
					}
				},
				_notifySingle(key, type, message, options) {

					message = this._tryToString(message);

					// If the entry does not exists, create it
					let index = this._find(key);
					if (index == -1) {
						this.entries.push({
							/**
							 * Time to display this item on the screen (in seconds)
							 */
							timeOnScreen: 5,
						});
						index = this.entries.length - 1;
					}

					// Method splice is necessary to make it reactive
					const entry = Object.assign(this.entries[index], options, {
						type: type,
						message: message,
						key: key
					});
					this.entries.splice(index, 1, entry);

					// If null, delete the entry
					if (message === null) {
						this.close(entry);
					}
					else if (entry.timeOnScreen > 0) {
						setTimeout(() => {
							this.close(entry);
						}, entry.timeOnScreen * 1000);
					}
				},
				/**
				 * Find en entry with this key
				 */
				_find(key) {
					for (let index in this.entries) {
						if (this.entries[index].key == key) {
							return index;
						}
					}
					return -1;
				},
				info(message, options) {
					this._notify("info", message, options);
				},
				error(message, options) {
					this._notify("error", message, Object.assign({
						timeOnScreen: 0
					}, options));
				},
				success(message, options) {
					this._notify("success", message, options);
				},
				close(entry) {
					const index = this.entries.indexOf(entry);
					if (index != -1) {
						this.entries.splice(index, 1);
					}
				}
			}
		});
	}
};
