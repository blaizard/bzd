const Web = require("../../../platform/nodejs/require/web.js");
const Exception = require("../../../platform/nodejs/require/exception.js")("backend");
const Commander = require("commander");

Commander.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("-p, --port <number>", "Port to be used to serve the application.", 8080, parseInt)
	.option("-s, --static <path>", "Directory to static serve.", "frontend")
	.parse(process.argv);

Exception.assert(Commander.port && parseInt(Commander.port) > 0, "Missing --port argument or it is not valid.");
Exception.assert(Commander.static, "Missing --static argument or it is not valid.");

// Set-up the web server
let web = new Web(Commander.port, {
	rootDir: Commander.static
});
web.start();

console.log("Started");
