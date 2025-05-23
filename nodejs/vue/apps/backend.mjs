import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import MockHttpServer from "#bzd/nodejs/core/http/mock/server.mjs";
import Authentication from "#bzd/apps/accounts/authentication/server.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

/// Backend server object to be used in the application.
export default class Backend {
	constructor(test) {
		this.instances = {
			authentication: null,
			rest: null,
			web: null,
		};
		this.restOptions = null;
		this.test = test;
		this.isSetup = false;
	}

	// Set-up the backend object.
	static make(test) {
		return new Backend(test);
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

	/// Set-up the authentication object.
	useAuthentication(options) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		this.instances.authentication = Authentication.make(options);
		return this;
	}

	/// Set-up the rest object.
	useRest(options) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		this.restOptions = options;
		return this;
	}

	/// Set-up the web server.
	setup(port, staticPath) {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		this.isSetup = true;

		// Set-up the web server.
		this.instances.web = this.test ? new MockHttpServer() : new HttpServer(port);
		if (staticPath) {
			this.instances.web.addStaticRoute("/", staticPath);
			Log.info("Serving static content from '{}'.", staticPath);
		}

		if (this.restOptions) {
			this.instances.rest = new RestServer(this.restOptions, {
				authentication: this.instances.authentication || null,
				channel: this.instances.web,
			});
			if (this.instances.authentication) {
				this.instances.rest.installPlugins(this.instances.authentication);
			}
		}

		return this;
	}

	/// Start the web server.
	async start() {
		Exception.assert(this.isSetup, "Backend not set-up.");
		Log.info("Application started");
		await this.instances.web.start();
	}
}
