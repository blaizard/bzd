import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Terminal from "#bzd/nodejs/vue/components/terminal/backend/terminal.mjs";

const Exception = ExceptionFactory("terminal", "websocket");
const Log = LogFactory("terminal", "websocket");

export default class TerminalWebsocket {
	installWebsocket(comms) {
		comms.handleWebsocket("/socket/terminal", (ws) => {
			let terminal = new Terminal("/");
			terminal.on("data", (data) => {
				ws.send(data);
			});
			terminal.on("exit", () => {
				ws.close();
			});
			ws.on("message", async (event) => {
				const input = JSON.parse(event.toString());
				switch (input.type) {
					case "init":
						await terminal.init(input.value);
						break;
					case "stream":
						await terminal.write(input.value);
						break;
					default:
						Log.error("Unsupported data type '{}' for terminal.", input.type);
				}
			});
		});
	}
}
