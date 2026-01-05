import WebSocket from "ws";

import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("http", "client", "node.ws");

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
			this.socket.terminate();
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
	if (url.startsWith("unix://")) {
		url = "ws+unix:" + url.replace("unix://", "", 1) + ":";
	}
	const ws = new WebSocket(url + options.path);

	return new Promise((resolve, reject) => {
		const context = new WebsocketContext(ws);

		ws.on("error", (error) => {
			if (reject) {
				reject(error);
				ws.close();
			}
		});

		ws.on("open", () => {
			resolve(context);
			reject = null;
		});

		ws.on("message", async (data) => {
			await callback(data.toString());
		});

		const heartbeatTracker = setInterval(() => {
			if (ws.readyState === WebSocket.OPEN) {
				ws.ping();
			} else {
				clearInterval(heartbeatTracker);
			}
		}, 30000);

		ws.on("close", () => {
			clearInterval(heartbeatTracker);
			context.close();
		});
	});
}
