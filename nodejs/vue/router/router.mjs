import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import Router from "../../core/router.mjs";

import RouterComponent from "./router_component.vue";
import RouterLink from "./router_link.vue";

const Log = LogFactory("router");
const Exception = ExceptionFactory("router");

Log.setLevel("debug");

async function delayMs(time) {
	return new Promise((resolve) => {
		setTimeout(() => {
			resolve();
		}, time);
	});
}

class RouterManager {
	constructor(options) {
		this.options = Object.assign(
			{
				/**
				 * Hash mode
				 */
				hash: true,
				/**
				 * Authentication object to be used with this router.
				 */
				authentication: null,
			},
			options,
		);

		this.routers = new Map();
		this.path = "/";
		this.regexMatchPath = new RegExp(
			this.options.hash ? "^[^#]+(?:#([^?]+))?(?:\\?.*)?$" : "^.*://[^/]+/([^?#]*)(?:[?#].*)?$",
		);
	}

	_getUid(vueElt) {
		return String(vueElt._uid);
	}

	/**
	 * Create a new router
	 */
	_makeRouter(vueElt, options) {
		let router = new Router({
			fallback: (path) => {
				Log.error("Route '{}' not found", path);
			},
		});

		options.routes.forEach((route) => {
			const handler = async (route, args, path, routerOptions) => {
				// Check if the route requires authentication,
				// if so ensure that we are authenticated
				if (route.authentication) {
					Exception.assert(
						this.options.authentication,
						"This route has authentication requirement but no authentication object was specified.",
					);
					await this.options.authentication.refreshAuthentication();
				}

				if ("component" in route) {
					vueElt.$refs[options.ref].componentSet(
						route.component,
						this._getUid(vueElt),
						Object.assign({}, args, routerOptions.props),
					);
				}
				if (route.handler) {
					route.handler(path, routerOptions, args);
				}
			};

			router.add(route.path, (args, routerOptions) => {
				handler(route, args, "/", routerOptions);
			});

			// To handle nested components
			if (route.nested) {
				router.add(route.path + "/{bzd.core.router.rest:.*}", (args) => {
					const rest = args["bzd.core.router.rest"];
					delete args["bzd.core.router.rest"];
					handler(route, args, "/" + rest);
				});
			}
		});

		return router;
	}

	/**
	 * Register a new router
	 */
	async registerRouter(vueElt, options) {
		const uid = this._getUid(vueElt);

		Exception.assert(!this.routers.has(uid), "A router '{}' is already registered for this element", uid);

		this.routers.set(uid, {
			router: this._makeRouter(vueElt, options),
			children: new Set(),
			parent: null,
			path: null,
			pathPropagate: null,
		});

		// Build component hierarchy
		const parent = vueElt.$el.closest("*[data-bzd-router-id]");
		if (parent) {
			const parentId = parent.getAttribute("data-bzd-router-id");
			this.routers.get(parentId).children.add(uid);
			this.routers.get(uid).parent = parentId;
		}

		Log.debug(
			"Registered router '{}' with parent '{}' (nb routers: {})",
			uid,
			this.routers.get(uid).parent,
			this.routers.size,
		);

		await this._propagate(uid);
	}

	/**
	 * Unregister a previously registered router
	 */
	unregisterRouter(uid) {
		// Unregister all the children (note, the list must be copied as elements will be deleted)
		const children = new Set(this.routers.get(uid).children);
		children.forEach((childUid) => {
			this.unregisterRouter(childUid);
		});
		if (this.routers.get(uid).parent) {
			let parent = this.routers.get(this.routers.get(uid).parent);
			Exception.assert(
				parent.children.delete(uid),
				"Child '{}' was not registered in parent '{}'",
				uid,
				this.routers.get(uid).parent,
			);
		}
		this.routers.delete(uid);

		Log.debug("Unregistered router '{}'", uid);
	}

	/**
	 * Get the current route or return null if none is detected
	 */
	getRoute(path = null) {
		if (path === null) {
			const match = window.location.href.match(this.regexMatchPath);
			if (match) {
				path = match[1];
			}
		}
		return (
			"/" +
			(path || "")
				.split("/")
				.filter((item) => item)
				.join("/")
		);
	}

	/**
	 * Dispatch a new path to the routers
	 */
	async dispatch(path = null, options = {}) {
		this.path = this.getRoute(path);

		options = Object.assign(
			{
				/**
				 * Query to be added to the URL.
				 */
				query: {},
				/**
				 * Custom properties that will be added to the component.
				 */
				props: {},
			},
			options,
		);

		// Update the url
		const queryStr = Object.keys(options.query).length
			? "?" +
			  Object.keys(options.query)
			  	.map((key) => key + "=" + encodeURIComponent(options.query[key]))
			  	.join("&")
			: "";
		history.pushState(null, null, this.options.hash ? queryStr + "#" + this.path : this.path + queryStr);

		// Clear all previously processed path
		for (const [, /*uid*/ config] of this.routers.entries()) {
			config.path = null;
			config.pathPropagate = null;
		}

		Log.debug("Dispatching path '{}'", this.path);

		// Propagate dispatched request to all top level routers
		let promiseList = [];
		for (const [uid, config] of this.routers.entries()) {
			if (config.parent == null) {
				promiseList.push(this._propagate(uid, options));
			}
		}
		await Promise.all(promiseList);
	}

	/**
	 * Propagate a dispatch request to the routers
	 */
	async _propagate(uid, options = {}) {
		let router = this.routers.get(uid);

		// If this router is already processed, do nothing
		if (router.path !== null) {
			return;
		}

		let data = null;
		// If this is a top level router
		if (router.parent === null) {
			data = await router.router.dispatch(this.path, options);
		}
		// Otherwise it must be a nested router
		else {
			const parent = this.routers.get(router.parent);
			Exception.assert(parent, "Router '{}' is set as nested but has an invalid parent '{}'", uid, router.parent);
			Exception.assert(parent.pathPropagate !== null, "Propagated path from parent '{}' is null", router.parent);
			data = await router.router.dispatch(parent.pathPropagate, options);
		}

		Exception.assert(data !== null, "Dispatched information is empty.");

		// Update the path of the router
		router.pathPropagate = "/" + (data.vars["bzd.core.router.rest"] || "");
		router.path = data.path.substr(
			0,
			data.path.length - router.pathPropagate.length + (router.pathPropagate == "/" ? 1 : 0),
		);

		Log.debug(
			"Router '{}' processed path '{}' (with vars {:j}) and propagate path '{}'",
			uid,
			router.path,
			data.vars,
			router.pathPropagate,
		);

		// Propagate to nested routers.
		// Timeout is to ensure routers are unregisters in the mean time.
		await delayMs(1);

		let promiseList = Array.from(router.children).map(async (childUid) => {
			await this._propagate(childUid, options);
		});
		await Promise.all(promiseList);
	}
}

export default class {
	static install(Vue, options) {
		Vue.component("RouterComponent", RouterComponent);
		Vue.component("RouterLink", RouterLink);

		let routers = new RouterManager(options);

		Vue.prototype.$routerSet = async function (routeOptions) {
			// Register hook
			// https://vuejs.org/v2/guide/components-edge-cases.html#Programmatic-Event-Listeners
			this.$once("hook:beforeDestroy", () => {
				const uid = routers._getUid(this);
				routers.unregisterRouter(uid);
			});

			await routers.registerRouter(this, routeOptions);
		};

		Vue.prototype.$routerDispatch = async (path, options = {}) => {
			await routers.dispatch(path, options);
		};

		Vue.prototype.$routerGet = () => {
			return routers.getRoute();
		};

		routers.dispatch();
	}
}
