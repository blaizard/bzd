"use strict";

export default {
    install(Vue) {
		Vue.prototype.$notification = new Vue({
			data: {
                entries: [],
                _uid: 0
            },
            methods: {
                _notify(type, message, options = {}) {
                    const entry = Object.assign({
                        /**
                         * Time to display this item on the screen (in seconds)
                         */
                        timeOnScreen: 5,
                    }, options, {
                        type: type,
                        message: message,
                        key: ++this._uid
                    })
                    this.entries.push(entry);
                    if (entry.timeOnScreen > 0) {
                        setTimeout(() => {
                            this.close(entry);
                        }, entry.timeOnScreen * 1000);
                    }
                },
                info(message, options) {
                    this._notify("info", message, options);
                },
                error(message, options) {
                    if (message instanceof Error) {
                        const splittedMessage = String(message).split("\n");
                        message = splittedMessage[0] || "";
                        if (splittedMessage.length > 1) {
                            message += "..."
                        }
                    }
                    this._notify("error", message, Object.assign({
                        timeOnScreen: 0
                    }, options));
                    console.error(message);
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
