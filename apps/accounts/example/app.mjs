import { program } from "commander";

import HttpServer from "#bzd/nodejs/core/http/server.mjs";

program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("-p, --port <number>", "Port to be used to serve the application.", 8081, parseInt)
	.option("-a, --accounts <url>", "URL of the accounts page.", ".")
	.parse(process.argv);

const options = program.opts();
const PORT = Number(options.port);
const ACCOUNTS_URL = options.accounts;

(async () => {
	const web = new HttpServer(PORT);

	web.addRoute("GET", "/", (context) => {
		context.send(
			'<head><body><a href="' + ACCOUNTS_URL + "/login?redirect=http://localhost:" + PORT + '">Login</a></body></head>',
		);
	});

	web.start();
})();
