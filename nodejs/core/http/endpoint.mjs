import ExceptionFactory from "../exception.mjs";
import { regexprEscape } from "#bzd/nodejs/utils/regexpr.mjs";

const Exception = ExceptionFactory("http", "server", "endpoint");

/// Parse an endpoint.
///
/// Valid schema are as follow:
/// /hello/{world}/my/{path:*}
export default class HttpEndpoint {
	constructor(endpoint) {
		const endpointSplited = endpoint.split("/").filter(Boolean);
		this.fragments = endpointSplited.map((fragment, index) => {
			const match = fragment.match(/^{([^}:]+)(:([^}]))?}$/);
			if (!match) {
				Exception.assert(
					!(fragment.includes("{") || fragment.includes("}")),
					"Endpoint string '{}' is malformed, this part: '{}'.",
					endpoint,
					fragment,
				);
				return fragment;
			}
			const isVarArgs = match[3] == "*";
			Exception.assert(
				!isVarArgs || index == endpointSplited.length - 1,
				"The variable argument part of the endpoint '{}' must be the last.",
				endpoint,
			);
			return {
				name: match[1],
				isVarArgs: isVarArgs,
			};
		});
	}

	/// Check if this endpoint has a variable argument.
	///
	/// Note, the variable argument must be last.
	isVarArgs() {
		return this.fragments[this.fragments.length - 1].isVarArgs;
	}

	map(callback) {
		return this.fragments.map(callback);
	}

	toRegexp() {
		const regexpEndpoint = this.fragments
			.map((fragment) => {
				if (typeof fragment == "string") {
					return regexprEscape(fragment);
				}
				if (fragment.isVarArgs) {
					// Can be empty.
					return "(?<" + fragment.name + ">.*)";
				}
				return "(?<" + fragment.name + ">[^/]+)";
			})
			.join("\\/");
		return new RegExp(regexpEndpoint, "i");
	}

	mapValues(values) {
		return (
			"/" +
			this.fragments
				.map((fragment) => {
					if (typeof fragment == "string") {
						return fragment;
					}
					return values[fragment.name];
				})
				.join("/")
		);
	}
}
