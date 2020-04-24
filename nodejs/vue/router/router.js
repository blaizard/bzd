import RouterComponent from "./router_component.vue";
import RouterLink from "./router_link.vue";
import Router from "../../core/router.js";
import LogFactory from "../../core/log.js";
import ExceptionFactory from "../../core/exception.js";

const Log = LogFactory("router");
const Exception = ExceptionFactory("router");

Log.setMinLevel("debug");

class RouterManager {

	constructor() {
		this.routers = new Map();
		this.path = "/";
		this.regexMatchPath = new RegExp("^[^#]+(?:#([^?]+))?(?:\\?.*)?$"); 
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
			}
		});

		options.routes.forEach((route) => {

			const handler = (path) => {
				if (route.component) {
					vueElt.$refs[options.ref].componentSet(route.component, this._getUid(vueElt));
				}
				if (route.handler) {
					route.handler(path);
				}
			};

			router.add(route.path, () => {
				handler(route, "/");
			});

			// To handle nested components
			if (route.nested) {
				router.add(route.path + "/{bzd.core.router.rest:.*}", (args) => {
					handler(route, "/" + args["bzd.core.router.rest"]);
				});
			}
		});

		return router;
	}

	/**
	 * Register a new router
	 */
	registerRouter(vueElt, options) {

		const uid = this._getUid(vueElt);

		Exception.assert(!this.routers.has(uid), "A router '{}' is already registered for this element", uid);

		this.routers.set(uid, {
			router: this._makeRouter(vueElt, options),
			children: new Set(),
			parent: null,
			path: null,
			pathPropagate: null
		});

		// Build component hierarchy
		const parent = vueElt.$el.closest("*[data-bzd-router-id]");
		if (parent) {
			const parentId = parent.getAttribute("data-bzd-router-id");
			this.routers.get(parentId).children.add(uid);
			this.routers.get(uid).parent = parentId;
		}

		Log.debug("Registered router '{}' with parent '{}' (nb routers: {})", uid, this.routers.get(uid).parent, this.routers.size);

		this._propagate(uid);
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
			Exception.assert(parent.children.delete(uid), "Child '{}' was not registered in parent '{}'", uid, this.routers.get(uid).parent);
		}
		this.routers.delete(uid);

		Log.debug("Unregistered router '{}'", uid);
	}

	/**
	 * Dispatch a new path to the routers
	 */
	dispatch(path = null) {

		// Read the path from the url
		if (path === null) {
			const match = window.location.href.match(this.regexMatchPath);
			if (match) {
				path = match[1];
			}
		}

		// Cleanup the path
		this.path = "/" + (path || "").split("/").filter(item => item).join("/");

		// Update the url
		history.pushState(null, null, "#" + this.path);		

		// Clear all previously processed path
		for (const [/*uid*/, config] of this.routers.entries()) {
			config.path = null;
			config.pathPropagate = null;
		}

		Log.debug("Dispatching path '{}'", this.path);

		// Propagate dispatched request to all top level routers
		for (const [uid, config] of this.routers.entries()) {
			if (config.parent == null) {
				this._propagate(uid);
			}
		}
	}

	/**
	 * Propagate a dispatch request to the routers
	 */
	_propagate(uid) {
		let router = this.routers.get(uid);

		// If this router is already processed, do nothing
		if (router.path !== null) {
			return;
		}

		let data = null;
		// If this is a top level router
		if (router.parent === null) {
			data = router.router.dispatch(this.path);
		}
		// Otherwise it must be a nested router
		else {
			const parent = this.routers.get(router.parent);
			Exception.assert(parent, "Router '{}' is set as nested but has an invalid parent '{}'", uid, router.parent);
			Exception.assert(parent.pathPropagate !== null, "Propagated path from parent '{}' is null", router.parent);
			data = router.router.dispatch(parent.pathPropagate);
		}

		Exception.assert(data !== null, "Dispacthed information is empty.");

		// Update the path of the router
		router.pathPropagate = "/" + (data.vars["bzd.core.router.rest"] || "");
		router.path = data.path.substr(0, data.path.length - router.pathPropagate.length + ((router.pathPropagate == "/") ? 1 : 0));

		Log.debug("Router '{}' processed path '{}' and propagate path '{}'", uid, router.path, router.pathPropagate);

		// Propagate to nested routers.
		// Timeout is to ensure routers are unregisters in the mean time.
		setTimeout(() => {
			router.children.forEach((childUid) => {
				this._propagate(childUid);
			});
		}, 1);
	}
}

export default class {
	static install(Vue, /*options*/) {

		Vue.component("RouterComponent", RouterComponent);
		Vue.component("RouterLink", RouterLink);

		let routers = new RouterManager();

		Vue.prototype.$routerSet = function (options) {

			// Register hook
			// https://vuejs.org/v2/guide/components-edge-cases.html#Programmatic-Event-Listeners
			this.$once("hook:beforeDestroy", function() {
				const uid = routers._getUid(this);
				routers.unregisterRouter(uid);
			});

			routers.registerRouter(this, options);
		};

		Vue.prototype.$routerDispatch = function (path) {
			routers.dispatch(path);
		};

		routers.dispatch();
	}
}
