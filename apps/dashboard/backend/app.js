"use strict";

const Web = require("../../../nodejs/require/web.js");
const Exception = require("../../../nodejs/require/exception.js")("backend");
const Commander = require("commander");

const Config = require("./config.js");

Commander.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("-p, --port <number>", "Port to be used to serve the application.", 8080, parseInt)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.parse(process.argv);

(async () => {

	// Set-up the web server
	let web = new Web(Commander.port, {
		rootDir: Commander.static
	});

	let config = new Config();
	await config.waitReady();

	web.start();
})();