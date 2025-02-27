import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import Authentication from "#bzd/apps/accounts/authentication/server.mjs";
import Config from "#bzd/apps/accounts/example/config.json" with { type: "json" };

(async () => {
	const web = new HttpServer(8081);

	const authentication = new Authentication({
		accounts: Config.accounts,
	});

	const api = new RestServer(APIv1.rest, {
		authentication: authentication,
		channel: web,
	});
	api.installPlugins(authentication);

	web.addStaticRoute("/", process.argv[2]);
	web.start();
})();
