import { reactive, readonly } from "vue";

import ExceptionFactory from "../core/exception.mjs";
import LoggerFactory from "../core/log.mjs";

const Exception = ExceptionFactory("notification");
const Log = LoggerFactory("notification");

export default {
	install: (app) => {
		const data = reactive({
			entries: [],
		});
		let _uid = 0;
		const defaultActions = [{ id: "close", callback: (entry) => notificationClose(entry) }];
		const tryToString = (message) => {
			if (message instanceof Error) {
				const splittedMessage = String(message).split("\n");
				message = splittedMessage[0] || "";
				if (splittedMessage.length > 1) {
					message += "...";
				}
			}
			return message;
		};
		const notify = (type, message, options = {}, callback = () => {}) => {
			if (typeof tryToString(message) == "string") {
				notifySingle(++_uid, type, message, options, callback);
			} else if (typeof message == "object") {
				for (const key in message) {
					notifySingle(key, type, message[key], options, callback);
				}
			} else {
				Exception.unreachable("Unsupported message type: {:j}", message);
			}
		};
		// Find en entry with this key
		const entryFind = (key) => {
			for (let index in data.entries) {
				if (data.entries[index].key == key) {
					return index;
				}
			}
			return -1;
		};
		const notifySingle = (key, type, message, options, callback) => {
			const messageStr = tryToString(message);

			// If the entry does not exists, create it
			let index = entryFind(key);
			if (index == -1) {
				data.entries.push({
					// Time to display this item on the screen (in seconds)
					timeOnScreen: 5,
					// Actions associated with this action, if empty no action will be shown.
					// An action is an html key (which represents the text associated with this action)
					// and a callback.
					actions: [...defaultActions],
				});
				index = data.entries.length - 1;
			}

			// Method splice is necessary to make it reactive
			const entry = Object.assign(data.entries[index], options, {
				type: type,
				message: messageStr,
				raw: message,
				key: key,
			});
			data.entries.splice(index, 1, entry);

			// Call the callback if any.
			callback(message);

			// If null, delete the entry
			if (messageStr === null) {
				notificationClose(entry);
			} else if (entry.timeOnScreen > 0) {
				setTimeout(() => {
					notificationClose(entry);
				}, entry.timeOnScreen * 1000);
			}
		};

		const info = (message, options) => {
			notify("info", message, options, (m) => {
				Log.info("{}", m);
			});
		};
		const error = (message, options) => {
			notify(
				"error",
				message,
				Object.assign(
					{
						timeOnScreen: 0,
					},
					options,
				),
				(m) => {
					Log.error("{}", m);
				},
			);
		};
		const success = (message, options) => {
			notify("success", message, options, (m) => {
				Log.info("{}", m);
			});
		};
		const notificationClose = (entry) => {
			const index = data.entries.indexOf(entry);
			if (index != -1) {
				data.entries.splice(index, 1);
			}
		};

		app.config.globalProperties.$notification = {
			info,
			error,
			success,
			close: notificationClose,
			entries: readonly(data).entries,
			defaultActions: readonly(defaultActions),
		};
	},
};
