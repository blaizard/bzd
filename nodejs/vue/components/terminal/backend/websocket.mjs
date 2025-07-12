import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Command from "#bzd/nodejs/vue/components/terminal/backend/command.mjs";

const Exception = ExceptionFactory("terminal", "websocket");
const Log = LogFactory("terminal", "websocket");

export default class TerminalWebsocket {
	installWebsocket(comms) {
		comms.handleWebsocket("/socket/terminal", (ws) => {
			let terminal = new Command(["docker", "run", "-i", "--rm", "ubuntu", "/bin/bash", "--help"]);
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
						await terminal.start();
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
