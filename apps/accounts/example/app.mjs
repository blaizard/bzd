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
			'<head><body><a href="' +
				ACCOUNTS_URL +
				"/login?product=hello&redirect=http://localhost2:" +
				PORT +
				'%2Fredirect">Login</a></body></head>',
		);
	});

	// Get the refresh token
	// Note trasnfering the refresh_token through HTTPS get query is not recommanded for the following reasons:
	// 1. Mostly HTTP referrer leakage (an external image in the target page might leak the password[1])
	// 2. Password will be stored in server logs (which is obviously bad)
	// 3. History caches in browsers
	// To mitigate this, the refersh token is immediatly exchanged to a new refresh token.
	web.addRoute("GET", "/redirect", (context) => {
		// context.temp_code
		// await post(ACCOUNTS_URL/auth/refresh?refresh_token=asaas&rolling=??)

		context.setCookie("hello2", "fsdsd");
		context.redirect("/");
	});

	web.start();
})();
