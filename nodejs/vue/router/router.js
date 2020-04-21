import Router from "../../core/router.js"
import Vue from "vue"

const regexMatchPath = new RegExp('^[^#]+(?:#([^?]+))?(?:\\?.*)?$'); 

class BzdRouter {

	constructor(options, path = null) {
		// Set default options
		this.options = Object.assign({
			routes: [],
			// Fallback content
			fallback: {},
			componentSet: () => {}
		}, options);

		// The router object
		this.router = new Router({
			fallback: (path) => {
				console.log("ooops not found");
			}
		});

		// Setup the routes
		this.options.routes.forEach((route) => {

			const handler = (route, path) => {
				if (route.component) {
					this.options.componentSet(route.component, path);
				}
				if (route.handler) {
					route.handler(path);
				}
			};

			this.router.add(route.path, (args) => {
				handler(route, "/");
			});
			// To handle nested components
			if (route.nested) {
				this.router.add(route.path + "/{bzd.core.router.rest:.*}", (args) => {
					handler(route, "/" + args["bzd.core.router.rest"]);
				});
			}

		});
/*
		setTimeout(() => {
		console.log("path", path);
		this.dispatch(path);
		}, 500);*/
	}

	dispatch(path = null) {

		// Read the path from the url
		if (path === null) {
			const match = window.location.href.match(regexMatchPath);
			if (match) {
				path = match[1];
			}
		}

		// Cleanup the path
		path = "/" + (path || "").split("/").filter(item => item).join("/");

		// Update the url
		//history.pushState(null, null, "#" + path);

		// Do the actual dispatching
		return this.router.dispatch(path);
	}
};

import RouterComponent from "./router_component.vue";
import RouterLink from "./router_link.vue";

export default class {
	static install(Vue, options) {

		Vue.component("RouterComponent", Vue.extend({
			mixins: [RouterComponent],
			methods: {
				componentSet(component, routerId) {
					this.component = component;
					this.routerId = routerId;
				}
			}
		}));

		Vue.component("RouterLink", RouterLink);

		let currentRouters = [];

		Vue.prototype.$bzdRouter = function (options) {
			const nestedPath = this.$el.getAttribute("bzd-router-nested-path") || null;
			const router = new BzdRouter(Object.assign(options, {
				componentSet: (component, subNestedPath) => {
					this.$refs[options.ref].componentSet(component, subNestedPath);
				}
			}), nestedPath);

			// Keep track of main routers only
			if (!nestedPath) {
				currentRouters.push({
					object: this,
					router: router
				});
			}

			return router;
		};

		let active = new Map();
		let routers = new Map();

		Vue.prototype.$registerRouter = function (options) {

			// Register hook
			// beforeDestroy
			// https://vuejs.org/v2/guide/components-edge-cases.html#Programmatic-Event-Listeners
			this.$once('hook:beforeDestroy', function() {
				this.$unregisterRouter();
			});

			// Add this element
			routers[this._uid] = {
				router: new BzdRouter(Object.assign(options, {
					componentSet: (component) => {
						this.$refs[options.ref].componentSet(component, this._uid);
					}
				})),
				children: new Set(),
				parent: null,
				path: null,
				nestedPath: null
			};

			// Build component hierarchy
			let element = this;
			const parent = this.$el.closest("*[data-bzd-router-id]");
			if (parent) {
				const parentId = parseInt(parent.getAttribute("data-bzd-router-id"));
				routers[parentId].children.add(this._uid);
				routers[this._uid].parent = parentId;
			}

			console.log("registerRouter", this._uid, routers);

			// Execute active path

			let data = null;
			// If no parent, then use the path
			if (routers[this._uid].parent === null) {
				data = routers[this._uid].router.dispatch();
			}
			else {
				// Check if parent is already computed, then pass the rest
				const nestedPath = routers[routers[this._uid].parent].nestedPath;
				console.log("Execute", nestedPath, "from parent", routers[this._uid].parent);
				data = routers[this._uid].router.dispatch(nestedPath);
			}

			const rest = data.vars["bzd.core.router.rest"] || "";
			routers[this._uid].path = data.path.substr(0, data.path.length - rest.length);
			routers[this._uid].nestedPath = "/" + rest;
		};

		Vue.prototype.$unregisterRouter = function () {
			console.log("unregisterRouter", this._uid, routers);

			delete routers[this._uid];
		};

		/*{
			"/dashboard/dsad": {
				"/edit"
			}
		}*/

		Vue.prototype.$bzdRouterLink = function (path) {
			console.log("path", path);

			console.log(currentRouters);
			currentRouters.forEach((item) => {
				item.router.dispatch(path);
			});
		};
	}
};
