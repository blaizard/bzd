import { createApp } from "vue";

import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import RestPlugin from "#bzd/nodejs/vue/rest.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";
import Authentication from "#bzd/apps/accounts/authentication/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Metadata from "#bzd/nodejs/vue/metadata.mjs";
import config from "#bzd/nodejs/vue/apps/config.json" with { type: "json" };

const Exception = ExceptionFactory("frontend");
const Log = LogFactory("frontend");

/// Frontend client object to be used in the application.
export default class Frontend {
	constructor(app) {
		this.instances = {
			app: createApp(app),
			authentication: null,
			services: null,
			statistics: null,
		};
		this.restOptions = null;
		this.isSetup = false;
	}

	/// Access the web server.
	get app() {
		Exception.assert(this.isSetup, "Frontend not set-up.");
		Exception.assert(this.instances.app, "Application not set-up.");
		return this.instances.app;
	}

	// Set-up the frontend object.
	static make(app) {
		return new Frontend(app);
	}

	/// Set-up the authentication object.
	useAuthentication(options = config.authentication, authentication = null) {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
		Exception.assert(!this.instances.authentication, "Authentication already set-up.");
		if (authentication) {
			this.instances.authentication = authentication;
		} else {
			this.instances.authentication = Authentication.make(
				Object.assign(
					{
						unauthorizedCallback: async (needAuthentication) => {
							console.log("Unauthorized!", needAuthentication);
						},
					},
					options,
				),
			);
		}
		this.instances.app.use(AuthenticationPlugin, {
			authentication: this.instances.authentication,
		});
		return this;
	}

	/// Set-up the rest object.
	useRest(options) {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
		Exception.assert(!this.restOptions, "Rest already set-up.");
		this.restOptions = options;
		return this;
	}

	/// Set-up the metadata object.
	useMetadata(options) {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
		this.instances.app.use(
			Metadata,
			Object.assign(
				{
					sitemap: "/sitemap.xml",
				},
				options,
			),
		);
		return this;
	}

	/// Set-up the services object.
	useServices() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.services, "Services already set-up.");
		this.instances.services = true;
		return this;
	}

	/// Set-up the statistics object.
	useStatistics() {
		Exception.assert(this.isSetup == false, "Backend already set-up.");
		Exception.assert(!this.instances.statistics, "Statistics already set-up.");
		this.instances.statistics = true;
		return this;
	}

	/// Set-up the client.
	setup(plugins = []) {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
		this.isSetup = true;

		if (this.instances.authentication) {
			plugins.push(this.instances.authentication);
		}

		this.instances.app.use(Router, {
			hash: false,
			authentication: this.instances.authentication || null,
			plugins: plugins,
		});
		this.instances.app.use(Notification);

		if (this.restOptions) {
			Log.info("Setting up rest client");
			this.instances.app.use(RestPlugin, {
				schema: this.restOptions,
				authentication: this.instances.authentication || null,
				plugins: plugins,
			});
		}

		// Set flags for the application.
		this.instances.app.config.globalProperties.$bzdAppsFeatures = {
			authentication: Boolean(this.instances.authentication),
			services: this.instances.services || false,
			statistics: this.instances.statistics || false,
		};

		return this;
	}

	/// Mount the client.
	mount(selector) {
		Exception.assert(this.isSetup, "Frontend not set-up.");
		Log.info("Application mounted to '{}'.", selector);
		this.instances.app.mount(selector);
	}
}
