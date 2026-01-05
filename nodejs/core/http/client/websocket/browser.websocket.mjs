import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("http", "client", "browser.websocket");
const Log = LogFactory("http", "client", "browser.websocket");

class WebsocketContext {
	constructor(socket) {
		this.socket = socket;
	}

	send(data) {
		if (this.socket) {
			this.socket.send(data);
		}
	}

	close() {
		if (this.socket) {
			this.socket.close();
			this.socket = null;
		}
	}

	onError(callback) {
		if (this.socket) {
			this.socket.on("error", callback);
		}
	}

	onClose(callback) {
		if (this.socket) {
			this.socket.on("close", callback);
		}
	}
}

export default async function handle(url, callback, options) {
	return new Promise((resolve, reject) => {
		const socket = new WebSocket(url + options.path);
		const context = new WebsocketContext(socket);
		socket.onopen = () => {
			resolve(context);
			reject = null;
		};
		socket.onmessage = async (event) => {
			await callback(event.data);
		};
		socket.onclose = (object) => {
			Exception.assert(
				// 1008 is used for errors on the server side, so we want to show it.
				object.wasClean && object.code != 1008,
				"Endpoint '{}' disconnected with code {} and reason: {}.",
				url,
				object.code,
				object.reason,
			);
			context.close();
		};
		socket.onerror = (error) => {
			if (reject) {
				reject(error);
				socket.close();
			}
		};
	});
}
