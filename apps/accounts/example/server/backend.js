import HttpServer from "#bzd/nodejs/core/http/server.js";
import APIv1 from "#bzd/api.json" with { type: "json" };
import RestServer from "#bzd/nodejs/core/rest/server.js";
import Authentication from "#bzd/apps/accounts/authentication/server.js";
import { configAccounts } from "#bzd/apps/accounts/example/config_nodejs.js";

(async () => {
	const web = new HttpServer(8081);

	const authentication = new Authentication({
		accounts: configAccounts(),
	});

	const api = new RestServer(APIv1.rest, {
		authentication: authentication,
		channel: web,
	});
	api.installPlugins(authentication);

	web.addStaticRoute("/", process.argv[2]);
	web.start();
})();
