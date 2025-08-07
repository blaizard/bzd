import LogFactory from "#bzd/nodejs/core/log.mjs";

const Log = LogFactory("terminal", "backend");

export default function install(context, command) {
	const onData = (data) => {
		context.send(data);
	};

	// Send data to the client.
	command.on("data", onData);
	context.exit(() => {
		command.remove("data", onData);
	});

	context.read((data) => {
		const input = JSON.parse(data.toString());
		switch (input.type) {
			case "init":
				// ignore init in this configuration.
				break;
			case "stream":
				command.write(input.value);
				break;
			default:
				Log.error("Unsupported data type '{}' for terminal.", input.type);
		}
	});
}
