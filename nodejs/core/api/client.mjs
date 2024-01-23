import ExceptionFactory from "../exception.mjs";
import HttpClient from "../http/client.mjs";
import HttpEndpoint from "#bzd/nodejs/core/http/endpoint.mjs";
import Validation from "../validation.mjs";

import Base from "./base.mjs";

const Exception = ExceptionFactory("api", "client");

export default class APIClient extends Base {
	constructor(schema, options) {
		super(schema, options);
		this.providers = {};
	}

	updateForm(method, endpoint, formDescription) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const validation = requestOptions.validation || {};

		return formDescription.map((description) => {
			if (description.name && description.name in validation) {
				description.validation =
					(description.validation ? description.validation + " " : "") + validation[description.name];
			}
			return description;
		});
	}

	/// Register a new provider.
	provide(key, action) {
		Exception.assert(!(key in this.providers), "The provider '{}' is already registered.", key);
		this.providers[key] = action;
	}

	/// Invoke a provider previously registered.
	async invoke(key, ...args) {
		Exception.assert(key in this.providers, "The provider '{}' is not registered.", key);
		return await this.providers[key](...args);
	}

	async login(uid, password, persistent = false, identifier = "") {
		Exception.assert(this.isAuthentication(), "Authentication is not enabled.");
		return await this.options.authentication.login(this, uid, password, persistent, identifier);
	}

	async loginWithSSO(ssoToken) {
		Exception.assert(this.isAuthentication(), "Authentication is not enabled.");
		return await this.options.authentication.loginWithSSO(this, ssoToken);
	}

	async loginWithAPI(method, path, data) {
		Exception.assert(this.isAuthentication(), "Authentication is not enabled.");
		return await this.options.authentication.loginWithAPI(this, method, path, data);
	}

	async logout() {
		Exception.assert(this.isAuthentication(), "Authentication is not enabled.");
		return await this.options.authentication.logout(this);
	}

	async request(method, endpoint, data = {}) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		// Build the options
		let fetchOptions = {
			method: method,
		};
		if ("type" in responseOptions) {
			fetchOptions.expect = responseOptions.type;
		}

		if ("validation" in requestOptions) {
			Exception.assert(
				["json", "query"].includes(requestOptions.type),
				"{} {}: validation is not available for {}.",
				method,
				endpoint,
				requestOptions.type,
			);
			const validation = new Validation(requestOptions.validation);
			validation.validate(data, {
				all: true,
			});
		}

		// Update the endpoint if needed.
		const updatedEndpoint = new HttpEndpoint(endpoint).mapValues(data);

		switch (requestOptions.type) {
			case "json":
				// Enforce best practice
				Exception.assert(
					method != "get",
					"{}: Body cannot be set with method '{}', this is against recommendation in the HTTP/1.1 spec, section 4.3",
					endpoint,
					method,
				);
				Exception.assert(typeof data === "object", "Data must be of type 'object', got '{:j}' instead.", data);
				fetchOptions.json = data;
				break;
			case "query":
				Exception.assert(typeof data === "object", "Data must be of type 'object', got '{:j}' instead.", data);
				fetchOptions.query = data;
				break;
			case undefined:
				break;
			default:
				Exception.unreachable("{} {}: Unsupported request type '{}'", method, endpoint, requestOptions.type);
		}

		// Check if this is a request that needs authentication
		if (this.schema[endpoint][method].authentication) {
			Exception.assert(
				this.isAuthentication(),
				"This route has authentication requirement but no authentication object was specified.",
			);
			Exception.assert(await this.options.authentication.isAuthenticated(), "A user must be authenticated.");
			await this.options.authentication.setAuthenticationFetch(fetchOptions);
		}

		const channel = this.options.channel ? this.options.channel : HttpClient;
		const result = await channel.request(this.getEndpoint(updatedEndpoint), fetchOptions);
		if ("validation" in responseOptions) {
			Exception.assert(
				["json"].includes(responseOptions.type),
				"{} {}: validation is not available for {}.",
				method,
				endpoint,
				responseOptions.type,
			);
			const validation = new Validation(responseOptions.validation);
			validation.validate(result, {
				all: true,
			});
		}
		return result;
	}
}
