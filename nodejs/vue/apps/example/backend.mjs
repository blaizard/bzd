import APIv1 from "#bzd/api.json" with { type: "json" };
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";

(async () => {
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useServices()
		.useCache()
		.useStatistics()
		.setup();

	await backend.start();
})();
