import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import MockHttpServer from "#bzd/nodejs/core/http/mock/server.mjs";
import Authentication from "#bzd/apps/accounts/authentication/server.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";
import Cache2 from "#bzd/nodejs/core/cache2.mjs";
import Statistics from "#bzd/nodejs/core/statistics/statistics.mjs";
import config from "#bzd/nodejs/vue/apps/config.json" with { type: "json" };

import { Command } from "commander/esm.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

/// Backend server object to be used in the application.
export default class Backend {
	constructor(port, test) {
		this.instances = {
			authentication: null,
			rest: null,
			web: test ? new MockHttpServer() : new HttpServer(port),
			cache: null,
			statistics: null,
			services: null,
			staticPath: null,
		};
		this.restOptions = null;
		this.isSetup = false;
		this.test = test;
	}

	// Set-up the backend object.
	static make(port, test) {
		return new Backend(port, test);
	}

	// Set-up the backend object from cli.
	static makeFromCli(argv) {
		const program = new Command();
		program
			.version("1.0.0", "-v, --version")
			.usage("[OPTIONS]...")
			.option(
				"-p, --port <number>",
				"Port to be used to serve the application, can also be set with the environment variable BZD_PORT.",
				8080,
			)
			.option("-s, --static <path>", "Directory to static serve.", ".")
			.option("--test", "Set the application in test mode.")
			.parse(argv);

		const port = parseInt(process.env.BZD_PORT || program.opts().port);
		const backend = new Backend(port, program.opts().test);
		backend.useStaticContent(program.opts().static);
		return backend;
	}

	/// Access the web server.
	get web() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.web, "Web server not set-up.");
		return this.instances.web;
	}

	/// Access the rest server.
	get rest() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.rest, "Rest server not set-up.");
		return this.instances.rest;
	}

	/// Access the authentication object.
	get authentication() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.authentication, "Authentication not set-up.");
		return this.instances.authentication;
	}

	/// Access the cache object.
	get cache() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.cache, "Cache not set-up.");
		return this.instances.cache;
	}

	/// Access the services object.
	get services() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.services, "Services not set-up.");
		return this.instances.services;
	}

	/// Access the statistics object.
	get statistics() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.statistics, "Statistics not set-up.");
		return this.instances.statistics;
	}

	/// Set-up the authentication object.
	useAuthentication(options = config.authentication) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.authentication, "Authentication already set-up.");
		this.instances.authentication = Authentication.make(options);
		return this;
	}

	/// Set-up the rest object.
	useRest(options) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.restOptions, "Rest already set-up.");
		this.restOptions = options;
		return this;
	}

	/// Set-up the cache object.
	useCache() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.cache, "Cache already set-up.");
		this.instances.cache = new Cache2();
		return this;
	}

	/// Set-up the services object.
	useServices() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.services, "Services already set-up.");
		this.instances.services = new Services();
		return this;
	}

	/// Set-up the statistics object.
	useStatistics() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.statistics, "Statistics already set-up.");
		this.instances.statistics = new Statistics();
		return this;
	}

	useStaticContent(path) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.staticPath, "Static content already set-up.");
		this.instances.staticPath = path;
		return this;
	}

	/// Set-up the web server.
	setup() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		this.isSetup = true;

		if (this.instances.services) {
			Log.info("Setting up services");
			if (this.instances.cache) {
				this.instances.services.register(this.instances.cache.serviceGarbageCollector("cache"), "backend");
			}
		}

		if (this.instances.statistics) {
			Log.info("Setting up statistics");
			if (this.instances.cache) {
				this.instances.statistics.register(this.instances.cache.statistics(), "backend");
			}
		}

		if (this.restOptions) {
			Log.info("Setting up rest server");
			this.instances.rest = new RestServer(this.restOptions, {
				authentication: this.instances.authentication || null,
				channel: this.instances.web,
			});
			if (this.instances.authentication) {
				this.instances.rest.installPlugins(this.instances.authentication);
			}
			if (this.instances.services) {
				this.instances.rest.installPlugins(this.instances.services);
			}
			if (this.instances.statistics) {
				this.instances.rest.installPlugins(this.instances.statistics);
			}
		}

		return this;
	}

	/// Start the web server.
	async start() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		if (this.instances.services) {
			Log.info("Starting services");
			await this.instances.services.start();
		}

		if (this.instances.staticPath) {
			// Important: the static path must be set after all other routes are registered as it will have the least priority.
			this.instances.web.addStaticRoute("/", this.instances.staticPath);
			Log.info("Serving static content from '{}'.", this.instances.staticPath);
		}

		Log.info("Starting web server");
		await this.instances.web.start();
	}

	async stop() {
		Log.info("Stopping web server");
		await this.instances.web.stop();
		if (this.instances.services) {
			Log.info("Stopping services");
			await this.instances.services.stop();
		}
	}
}
