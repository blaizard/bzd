import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import WebsocketServer from "#bzd/nodejs/core/websocket/server.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import MockHttpServer from "#bzd/nodejs/core/http/mock/server.mjs";
import Authentication from "#bzd/apps/accounts/authentication/server.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";
import Cache2 from "#bzd/nodejs/core/cache2.mjs";
import Statistics from "#bzd/nodejs/core/statistics/statistics.mjs";
import Form from "#bzd/nodejs/vue/components/form/backend.mjs";
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
			websocket: null,
			web: test ? new MockHttpServer() : new HttpServer(port),
			cache: null,
			statistics: null,
			services: null,
			form: null,
			staticPath: null,
			staticOptions: null,
			restSchema: null,
			restOptions: null,
			websocketSchema: null,
			websocketOptions: null,
		};
		this.isSetup = false;
		this.test = test;
	}

	// Set-up the backend object.
	static make(port, test) {
		return new Backend(port, test);
	}

	/// Set-up the backend object from cli.
	///
	/// \param {Array} argv - The command line arguments.
	/// \param {Command} command - The command object to use, if not provided a new one will be created.
	static makeFromCli(argv, command = null) {
		const program = command || new Command();
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

	/// Access the websocket server.
	get websocket() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.websocket, "Websocket server not set-up.");
		return this.instances.websocket;
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

	/// Access the form object.
	get form() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Exception.assert(this.instances.form, "Form not set-up.");
		return this.instances.form;
	}

	/// Set-up the authentication object.
	useAuthentication(options = config.authentication) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.authentication, "Authentication already set-up.");
		this.instances.authentication = Authentication.make(options);
		return this;
	}

	/// Set-up the rest object.
	useRest(schema, options) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.restSchema, "Rest schema already set-up.");
		Exception.assert(!this.instances.restOptions, "Rest options already set-up.");
		this.instances.restSchema = schema;
		this.instances.restOptions = options;
		return this;
	}

	/// Set-up the websocket object.
	useWebsocket(schema, options) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.websocketSchema, "Websocket schema already set-up.");
		Exception.assert(!this.instances.websocketOptions, "Websocket options already set-up.");
		this.instances.websocketSchema = schema;
		this.instances.websocketOptions = options;
		return this;
	}

	/// Set-up the cache object.
	useCache() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.cache, "Cache already set-up.");
		this.instances.cache = new Cache2("cache");
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

	/// Set-up the form object.
	useForm(options = null) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.form, "Form already set-up.");
		this.instances.form = new Form(options);
		return this;
	}

	useStaticContent(path, options = null) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.staticPath, "Static content already set-up.");
		this.instances.staticPath = path;
		if (options) {
			this.useStaticContentOptions(options);
		}
		return this;
	}

	useStaticContentOptions(options) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(this.instances.staticPath, "Static content must be set-up.");
		Exception.assert(!this.instances.staticOptions, "Static content options already set-up.");
		this.instances.staticOptions = options;
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
				this.instances.statistics.register(this.instances.cache.statistics, "backend");
			}
			if (this.instances.form) {
				this.instances.statistics.register(this.instances.form.statistics, "backend");
			}
			if (this.instances.form) {
				this.instances.statistics.register(this.instances.web.statistics, "backend");
			}
		}

		if (this.instances.restSchema) {
			Log.info("Setting up rest server");
			const restOptions = Object.assign(
				{
					authentication: this.instances.authentication || null,
					channel: this.instances.web,
				},
				this.instances.restOptions || {},
			);
			this.instances.rest = new RestServer(this.instances.restSchema, restOptions);

			if (this.instances.authentication) {
				this.instances.rest.installPlugins(this.instances.authentication);
			}
			if (this.instances.services) {
				this.instances.rest.installPlugins(this.instances.services);
			}
			if (this.instances.statistics) {
				this.instances.rest.installPlugins(this.instances.statistics);
			}
			if (this.instances.form) {
				this.instances.rest.installPlugins(this.instances.form);
			}
		}

		if (this.instances.websocketSchema) {
			Log.info("Setting up websocket server");
			const websocketOptions = Object.assign(
				{
					channel: this.instances.web,
				},
				this.instances.websocketOptions || {},
			);
			this.instances.websocket = new WebsocketServer(this.instances.websocketSchema, websocketOptions);
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

		if (this.instances.statistics) {
			Log.info("Starting statistics");
			await this.instances.statistics.start();
		}

		if (this.instances.staticPath) {
			// Important: the static path must be set after all other routes are registered as it will have the least priority.
			this.instances.web.addStaticRoute("/", this.instances.staticPath, this.instances.staticOptions || {});
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
		if (this.instances.statistics) {
			Log.info("Stopping statistics");
			await this.instances.statistics.stop();
		}
	}
}
