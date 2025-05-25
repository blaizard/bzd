import APIv1 from "#bzd/api.json" with { type: "json" };
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";

import { Command } from "commander/esm.mjs";

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with the environment variable BZD_PORT.",
		8080,
		parseInt,
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option("--test", "Set the application in test mode.")
	.parse(process.argv);

(async () => {
	const PORT = program.opts().port;
	const PATH_STATIC = program.opts().static;
	const TEST = program.opts().test;

	const backend = Backend.make(TEST)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useServices()
		.useCache()
		.useStatistics()
		.useStaticContent(PATH_STATIC)
		.setup(PORT);

	await backend.start();
})();
