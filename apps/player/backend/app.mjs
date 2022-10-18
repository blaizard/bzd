import { Command } from "commander/esm.mjs";

import API from "bzd/core/api/server.mjs";
import APIv1 from "../api.v1.json";
import HttpServer from "bzd/core/http/server.mjs";
import LogFactory from "bzd/core/log.mjs";
import FileSystem from "bzd/core/filesystem.mjs";

const Log = LogFactory("backend");

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with the environemnt variable BZD_PORT.",
		8080,
		parseInt
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.parse(process.argv);

class Scenario {
	constructor(data) {
		this.data = data;
		this.index = 0;
	}

	get() {
		return this.data;
	}
}

(async () => {
	// Read arguments
	const PORT = process.env.BZD_PORT || program.opts().port;
	const PATH_STATIC = program.opts().static;
	const PATH_SCENARIO = program.args[0];

	const raw = await FileSystem.readFile(PATH_SCENARIO);
	const scenarioData = JSON.parse(raw);

	const scenario = new Scenario(scenarioData);

	// Set-up the web server
	let web = new HttpServer(PORT);
	// Install the APIs
	let api = new API(APIv1, {
		channel: web,
	});

	api.handle("get", "/scenario", async () => {
		return scenario.get();
	});

	Log.info("Application started");
	web.addStaticRoute("/", PATH_STATIC, "index.html");
	web.start();
})();
