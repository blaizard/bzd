import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Router from "#bzd/nodejs/core/router.mjs";

import RouterComponent from "#bzd/nodejs/vue/router/router_component.vue";
import RouterLink from "#bzd/nodejs/vue/router/router_link.vue";
import Registry from "#bzd/nodejs/vue/router/registry.mjs";
import { onBeforeUnmount } from "vue";

const Log = LogFactory("router");
const Exception = ExceptionFactory("router");

class Path {
	constructor(route) {
		this.route = Path.clean(route);
		this.remaining = this.route;
		this.processed = "";
	}

	static makeFromLocation(hash) {
		return new Path(Path.getLocation(hash));
	}

	static clean(path) {
		return (
			"/" +
			(path || "")
				.split("/")
				.filter((item) => item)
				.join("/")
		);
	}

	static getLocation(hash) {
		const regexMatchPath = new RegExp(hash ? "^[^#]+(?:#([^?]+))?(?:\\?.*)?$" : "^.*://[^/]+/([^?#]*)(?:[?#].*)?$");

		let path = "";
		const match = window.location.href.match(regexMatchPath);
		if (match) {
			path = match[1];
		}
		return path;
	}

	static getQuery() {
		return window.location.search
			.replace("?", "")
			.split("&")
			.filter(Boolean)
			.reduce((object, entry) => {
				const [key, value] = entry.split("=");
				object[decodeURIComponent(key)] = decodeURIComponent(value);
				return object;
			}, {});
	}

	/// The path has remaining route to be processed.
	hasRemaining() {
		return Boolean(this.remaining);
	}

	/// Get the remaining part of the path to be processed.
	getRemaining() {
		return this.remaining;
	}

	setRemaining(route) {
		this.remaining = route;
	}

	/// Get the route that was triggered.
	getRoute() {
		return this.route;
	}
}

class RouterManager {
	constructor(registry, options) {
		this.registry = registry;
		this.options = Object.assign(
			{
				// Hash mode
				hash: true,
				// Authentication object to be used with this router.
				authentication: null,
				// The available routes, if set all added routes must match, otherwise this is ignored.
				schema: null,
				// Called each time a new route is processed.
				onRouteUpdate: (route) => {},
			},
			options,
		);

		this.routers = {};
		this.path = Path.makeFromLocation(this.options.hash);
		this.uid = 0;
		this.updateState(Path.getQuery());
	}

	registerRouter(configuration) {
		const handler = async (route, args, dispatchOptions) => {
			// Check if the route requires authentication,
			// if so ensure that we are authenticated
			if (route.authentication) {
				Exception.assert(
					this.options.authentication,
					"This route has authentication requirement but no authentication object was specified.",
				);
				await this.options.authentication.assertScopes(route.authentication);
			}

			if (route.component) {
				await this.registry.setComponent(
					configuration.component,
					route.component,
					Object.assign({}, args, dispatchOptions.props, route.props),
				);
			}

			if (route.handler) {
				await route.handler(route.path, dispatchOptions, args);
			}

			if (route.redirect) {
				await this.dispatch(route.redirect);
			}
		};

		let router = new Router({
			fallback: async (path) => {
				Log.error("Route '{}' not found", path);
				if (configuration.fallback) {
					await handler(configuration.fallback, {}, {});
				}
			},
		});

		configuration.routes.forEach((route) => {
			Exception.assert(route.path, "This route is missing a path: {:j}", route);
			if (this.options.schema !== null) {
				Exception.assert(route.path in this.options.schema, "The route '{}' is missing from the schema.", route.path);
				route = Object.assign({}, route, this.options.schema[route.path]);
			}

			const metadata = {
				authentication: route.authentication || false,
			};

			// TODO: add support for nested routes.
			//router.add(route.path + "/{bzd.core.router.rest:*}", (args, dispatchOptions) => {
			//    this.path.setRemaining("/" + args["bzd.core.router.rest"]);
			//    console.log("NESTED!", route.path, "/" + args["bzd.core.router.rest"]);
			//    delete args["bzd.core.router.rest"];
			//}, metadata);

			router.add(
				route.path,
				async (args, dispatchOptions) => {
					this.path.setRemaining("");
					await handler(route, args, dispatchOptions);
				},
				metadata,
			);
		});

		// Register the router
		const uid = ++this.uid;
		this.routers[uid] = router;
		return uid;
	}

	/// Trigger a specific router to try.
	async trigger(uid) {
		if (this.path.hasRemaining()) {
			Exception.assert(uid in this.routers, "This router is not registered.");
			await this.routers[uid].dispatch(this.path.getRemaining(), {});
		}
	}

	async unregisterRouter(uid) {
		delete this.routers[uid];
	}

	updateState(query) {
		// Update the url
		const queryStr = Object.keys(query).length
			? "?" +
				Object.keys(query)
					.map((key) => key + "=" + encodeURIComponent(query[key]))
					.join("&")
			: "";
		const state = this.options.hash ? queryStr + "#" + this.path.route : this.path.route + queryStr;
		history.pushState(null, null, state);

		// Call the hook.
		this.options.onRouteUpdate(this.path.route);
	}

	async dispatch(path = null, dispatchOptions = {}) {
		// Clear current actions if any.
		this.registry.reset();
		this.path = path ? new Path(path) : Path.makeFromLocation(this.options.hash);

		// Update the url.
		this.updateState(dispatchOptions.query || Path.getQuery());

		// Loop through the routers and dispatch.
		for (const router of Object.values(this.routers)) {
			if (!this.path.hasRemaining()) {
				break;
			}
			await router.dispatch(this.path.getRemaining(), dispatchOptions);
		}
	}

	/// Return meta-information about this route.
	///
	/// \param path The path to be checked.
	/// \return The information associated to the path, null if the path does not exists.
	getInfoFromPath(path) {
		for (const router of Object.values(this.routers)) {
			const match = router.match(path);
			if (match) {
				return Object.assign({ path: path }, match.args);
			}
		}
		return null;
	}

	/// Get the route that was triggered.
	getRoute() {
		return this.path.getRoute();
	}
}

export default {
	install: (app, options) => {
		app.component("RouterComponent", RouterComponent);
		app.component("RouterLink", RouterLink);

		let registry = new Registry();
		let routers = new RouterManager(registry, options);

		const api = {
			/// Register new routes.
			///
			/// It should be called from setup or onMounted.
			///
			/// \param configuration The configuration to set the routes.
			async set(configuration) {
				const uid = routers.registerRouter(configuration);
				onBeforeUnmount(() => {
					routers.unregisterRouter(uid);
				});
				await routers.trigger(uid);
			},
			/// Dispatch a specific path.
			///
			/// \param path The path to be dispatched onto the router.
			/// \param options The options to be passed.
			async dispatch(path, options = {}) {
				await routers.dispatch(path, options);
			},
			/// Get the current route.
			get() {
				return routers.getRoute();
			},
			/// Get the information about a certain path.
			fromPath(path) {
				return routers.getInfoFromPath(path);
			},
		};

		app.config.globalProperties.$router = api;
		app.provide("$router", api);
		app.provide("$routerRegistry", registry);
	},
};
