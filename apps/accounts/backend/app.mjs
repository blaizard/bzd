import { program } from "commander";
import Path from "path";

import API from "#bzd/nodejs/core/api/server.mjs";
import APIv1 from "#bzd/apps/accounts/api.v1.json" assert { type: "json" };
import KeyValueStoreDisk from "#bzd/nodejs/db/key_value_store/disk.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Authentication from "#bzd/nodejs/core/authentication/token/server.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with BZD_PORT.",
		8080,
		parseInt,
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option("-d, --data <path>", "Where to store the data, can also be set with BZD_PATH_DATA.", "/tmp/bzd/accounts")
	.parse(process.argv);

const options = program.opts();
const PORT = Number(process.env.BZD_PORT || options.port);
const PATH_DATA = process.env.BZD_PATH_DATA || options.data;
const PATH_STATIC = options.static;

(async () => {
	const keyValueStore = await KeyValueStoreDisk.make(Path.join(PATH_DATA, "db", "kvs"));

	// Set-up the web server
	const web = new HttpServer(PORT);

	web.addStaticRoute("/", PATH_STATIC, "index.html");
	web.start();
})();
