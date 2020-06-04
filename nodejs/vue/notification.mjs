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
                info(...args) {
                    this._notify("info", ...args);
                },
                error(...args) {
                    this._notify("error", ...args);
                },
                success(...args) {
                    this._notify("success", ...args);
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
