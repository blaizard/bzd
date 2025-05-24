import { createApp } from "vue";

import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import RestPlugin from "#bzd/nodejs/vue/rest.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";
import Authentication from "#bzd/apps/accounts/authentication/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Metadata from "#bzd/nodejs/vue/metadata.mjs";

const Exception = ExceptionFactory("frontend");
const Log = LogFactory("frontend");

/// Frontend client object to be used in the application.
export default class Frontend {
	constructor(app) {
		this.instances = {
			app: createApp(app),
			authentication: null,
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
	useAuthentication(options) {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
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
		this.instances.app.use(AuthenticationPlugin, {
			authentication: this.instances.authentication,
		});
		return this;
	}

	/// Set-up the rest object.
	useRest(options) {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
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

	/// Set-up the client.
	setup() {
		Exception.assert(this.isSetup == false, "Frontend already set-up.");
		this.isSetup = true;

		this.instances.app.use(Router, {
			hash: false,
			authentication: this.instances.authentication || null,
		});
		this.instances.app.use(Notification);

		if (this.restOptions) {
			Log.info("Setting up rest client");
			let plugins = [];
			if (this.instances.authentication) {
				plugins.push(this.instances.authentication);
			}
			this.instances.app.use(RestPlugin, {
				schema: this.restOptions,
				authentication: this.instances.authentication || null,
				plugins: plugins,
			});
		}

		return this;
	}

	/// Mount the client.
	mount(selector) {
		Exception.assert(this.isSetup, "Frontend not set-up.");
		Log.info("Application mounted to '{}'.", selector);
		this.instances.app.mount(selector);
	}
}
