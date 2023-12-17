import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import API from "#bzd/nodejs/core/api/server.mjs";
import AuthenticationProxy from "#bzd/nodejs/core/authentication/session/server_proxy.mjs";
import Config from "#bzd/apps/accounts/example/config.json" assert { type: "json" };

(async () => {
	const web = new HttpServer(8081);

	const authentication = new AuthenticationProxy({
		remote: Config.accounts,
	});

	const api = new API(APIv1, {
		authentication: authentication,
		channel: web,
	});
	await api.installPlugins(authentication);

	web.addStaticRoute("/", process.argv[2]);
	web.start();
})();
