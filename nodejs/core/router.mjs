import { regexprEscape } from "../utils/regexpr.mjs";

import ExceptionFactory from "./exception.mjs";

const Exception = ExceptionFactory("router");

const regexp_ = /{([^}:]+):?([^}]*)}/g;

export default class Router {
	constructor(config) {
		this.config = Object.assign(
			{
				/// If set, the route match will differentiate with upper and lower case.
				caseSensitive: false,
				/// Callback if no routes are matching.
				fallback: null,
			},
			config,
		);
		this.routes = new Map();
		this.dirty = true;
		this.data = null;
	}

	/// \brief Associate a new route with a handler (callback).
	/// It supports the following format:
	///	/api/robots
	///	/api/robots/search/{name} // match the next segment of the path (until the next slash)
	///	/api/robots/{id:i}        // match integer
	/// /api/robots/{rest:*}      // match all the rest of the path (including slashes)
	add(path, handler, args = {}) {
		Exception.assert(handler !== undefined, "You must define a handler for the route '{}'.", path);

		this.routes.set(path, {
			handler: handler,
			args: args,
		});
		this.dirty = true;
	}

	/// Try to match the given path
	match(path) {
		// Compile the routes once
		if (this.dirty) {
			this.data = compileRoutes(this.routes, this.config);
			this.dirty = false;
		}
		// Try to match the regexpr
		const matches = path.match(this.data.regexpr);
		if (matches === null) {
			return false;
		}

		// Identify the index
		let counter = matches.length;
		while (--counter && matches[counter] === undefined) {} // eslint-disable-line

		const indexRoute = Math.floor((counter - 1) / this.data.maxVars);
		const indexVar = indexRoute * this.data.maxVars;

		const route = this.data.routes[indexRoute];

		// Build the variables
		let vars = {};
		route.varList.forEach((varSchema, index) => {
			const varStr = matches[index + indexVar + 1];
			vars[varSchema.name] = varSchema.cast(varStr);
		});

		// Return the match
		return {
			route: route,
			args: route.args,
			vars: vars,
		};
	}

	/// \brief Handle the current request, with routes previously added.
	async dispatch(path, ...args) {
		const match = this.match(path);
		if (match) {
			await match.route.handler(match.vars, ...args);
			return {
				matched: true,
				path: path,
				vars: match.vars,
			};
		}

		if (this.config.fallback) {
			await this.config.fallback(path, ...args);
		}
		return false;
	}

	static toCompiledRoute(path, config = {}) {
		// Build the regular expression out of the path
		const pathList = path.split(regexp_);
		let pathRegexpr = regexprEscape(pathList[0]);
		let varList = [];
		let segments = [pathList[0]];
		for (let i = 1; i < pathList.length; i += 3) {
			let castType = String;
			let uncastType = String;

			// Update the regular expression of the path based on the format given
			switch (String(pathList[i + 1])) {
				// Match non-empty strings.
				case "":
					castType = decodeURIComponent;
					uncastType = encodeURIComponent;
					pathRegexpr += "([^/]+)";
					break;
				// Must be placed at the beginning or just after a slash.
				// Match everything after the slash and also match the empty path even without the slash:
				// - /hello/{:*} will match:
				//   - /hello  -> capture: ""
				//   - /hello/ -> capture: ""
				//   - /hello/manythigs/including/slashes -> capture: "manythigs/including/slashes"
				case "*":
					// No decodeURIComponent here, a raw string should be used here to differentiate between slashes and components.
					castType = (str) => str.substring(1);
					// If the last character is a '/'.
					if (pathRegexpr && pathRegexpr.at(-1) == "/") {
						pathRegexpr = pathRegexpr.slice(0, -1);
						pathRegexpr += "($|/.*)";
					} else {
						Exception.assert(!pathRegexpr, "Variable arguments must be set just after '/'.");
						pathRegexpr += "(.*)";
					}
					break;
				// Must be placed at the beginning or just after a slash.
				// Match only if there is something (non-empty) after the slash
				case "+":
					Exception.assert(!pathRegexpr || pathRegexpr.at(-1) == "/", "Variable arguments must be set just after '/'.");
					pathRegexpr += "(.+)";
					break;
				case "i":
					castType = parseInt;
					pathRegexpr += "([0-9]+)";
					break;
				default:
					Exception.unreachable("Unsupported router format: '{}' from route: {}", pathList[i + 1], path);
			}

			// Add the variable to the list
			varList.push({
				cast: castType,
				uncast: uncastType,
				name: pathList[i],
			});

			// Add the follow-up of the path if any
			if (pathList[i + 2]) {
				pathRegexpr += regexprEscape(pathList[i + 2]);
				segments.push(pathList[i + 2]);
			}
		}

		const regexpr = new RegExp("^" + pathRegexpr + "$", config.caseSensitive ? undefined : "i");
		return {
			regexpr: regexpr,
			regexprAsString: pathRegexpr,
			vars: varList,
			// Match the object against the given path.
			match: (path) => {
				const match = path.match(regexpr);
				if (match === null) {
					return false;
				}
				const vars = varList.map((variable, index) => {
					const str = match[index + 1];
					return [variable.name, variable.cast(str)];
				});
				return Object.fromEntries(vars);
			},
			// Map a set of variables to re-create the route.
			toRoute: (values) => {
				return segments
					.map((segment, index) => {
						if (varList[index]) {
							const name = varList[index].name;
							Exception.assert(name in values, "The value '{}' is missing.", name);
							segment += varList[index].uncast(values[name]);
						}
						return segment;
					})
					.join("");
			},
		};
	}
}

/// \brief Pre-process routes before being used.
function compileRoutes(routes, config) {
	// Create the master regexpr, to optimize the search speed
	let routeData = [];
	// This is used for future computing and will get the maximum number of variable
	let maxVars = 0;

	for (const [path, config] of routes.entries()) {
		const { regexprAsString, vars } = Router.toCompiledRoute(path, config);
		maxVars = Math.max(maxVars, vars.length);

		// Update the root data
		routeData.push({
			args: config.args,
			handler: config.handler,
			varList: vars,
			pathRegexpr: regexprAsString,
		});
	}

	// Collapse the number of regular expression to be able to identify the regexpr
	let regexprList = [];
	routeData.forEach((route /*index*/) => {
		// +1 to support cases where there are no variables
		const pathRegexpr = route.pathRegexpr + "()".repeat(maxVars - route.varList.length + 1);
		regexprList.push(pathRegexpr);
	});

	return {
		regexpr: new RegExp("^(?:|" + regexprList.join("|") + ")$", config.caseSensitive ? undefined : "i"),
		routes: routeData,
		maxVars: maxVars + 1,
	};
}
