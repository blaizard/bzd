"use strict";

import ExceptionFactory from "./exception.js";

const Exception = ExceptionFactory("router");

export default class Router {
	constructor(config) {
		this.config = Object.assign({
			/**
			 * \brief If set, the route match will differentiate with upper and lower case.
			 */
			caseSensitive: false,
			/**
			 * \brief Callback if no routes are matching.
			 */
			fallback: (path) => {
				throw new Exception("Route not found for '{}'.", path);
			}
		}, config);
		this.routes = new Map();
		this.dirty = true;
		this.data = null;
	}

	/** 
	 * \brief Associate a new route with a handler (callback).
	 * It supports the following format:
	 *	/api/robots
	 *	/api/robots/search/{name}
	 *	/api/robots/{id:[0-9]+}
	 */
	add(path, handler, args = {}) {
		Exception.assert(handler !== undefined, "You must define a handler for the route '{}'.", path);

		this.routes.set(path, {
			handler: handler,
			args: args
		});
		this.dirty = true;
	}

	/**
	 * Try to match the given path
	 */
	match(path) {
		// Compile the routes once
		if (this.dirty) {
			this.data = compileRoutes(this.routes, this.config);
			this.dirty = false;
		}

		// Try to match the regexpr
		const matches = path.match(this.data.regexpr);

		if (!matches) {
			return false;
		}

		// Identify the index
		let counter = matches.length;
		while (--counter && matches[counter] === undefined);

		const indexRoute = Math.floor((counter - 1) / this.data.maxVars);
		const indexVar = indexRoute * this.data.maxVars;

		const route = this.data.routes[indexRoute];

		// Build the variables
		let vars = {};
		route.varList.forEach((name, index) => {
			vars[name] = matches[index + indexVar + 1];
		});

		// Return the match
		return {
			route: route,
			args: route.args,
			vars: vars
		}
	}

	/**
	 * \brief Handle the current request, with routes previously added.
	 */
	dispatch(path, ...args) {

		const match = this.match(path);
		if (match)
		{
			match.route.handler(match.vars, ...args);
			return {
				matched: true,
				path: path,
				vars: match.vars
			}
		}

		this.config.fallback(path, ...args);
		return {
			matched: false,
			path: path,
			vars: {}
		}
    }
};

/**
 * \brief Escape a string to be used inside a regular expression.
 */
function escapeRegexp(str)
{
	return str.replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, "\\$&");
}

/**
 * \brief Pre-process routes before being used.
 */
function compileRoutes(routes, config)
{
	// Create the master regexpr, to optimize the search speed
	let routeData = [];
	// This is used for future computing and will get the maximum number of variable
	let maxVars = 0;
	// Regular expression used to extract variables
	const regexp = /{([^}:]+):?([^}]*)}/g;

	for (const [path, config] of routes.entries()) {

		let varList = [];

		// Build the regular expression out of the path
		const pathList = path.split(regexp);
		let pathRegexpr = escapeRegexp(pathList[0]);

		for (let i = 1; i < pathList.length; i += 3) {

			// Add the variable to the list
			varList.push(pathList[i]);

			// Update the regular expression of the path
			pathRegexpr += "(" + ((pathList[i + 1]) ? pathList[i + 1] : "[^/]+") + ")";

			// Add the follow-up of the path if any
			if (pathList[i + 2]) {
				pathRegexpr += escapeRegexp(pathList[i + 2]);
			}
		}

		maxVars = Math.max(maxVars, varList.length);

		// Update the root data
		routeData.push({
			args: config.args,
			handler: config.handler,
			varList: varList,
			pathRegexpr: pathRegexpr
		});
	}

	// Collapse the number of regular expression to be able to identify the regexpr
	let regexprList = [];
	routeData.forEach((route, index) => {
		// +1 to support cases where there are no variables
		const pathRegexpr = route.pathRegexpr + "()".repeat(maxVars - route.varList.length + 1);
		regexprList.push(pathRegexpr);
	});

	return {
		regexpr: new RegExp("^(?:|" + regexprList.join("|") + ")$", ((config.caseSensitive) ? undefined : "i")),
		routes: routeData,
		maxVars: maxVars + 1
	};
}
