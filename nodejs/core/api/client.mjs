

import Base from "./base.mjs";
import Fetch from "../fetch.mjs";
import ExceptionFactory from "../exception.mjs";
import ExceptionFetch from "../impl/fetch/exception.mjs";
import Validation from "../validation.mjs";

const Exception = ExceptionFactory("api", "client");

export default class APIClient extends Base {

	updateForm(method, endpoint, formDescription) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const validation = requestOptions.validation || {};

		return formDescription.map((description) => {
			if (description.name && description.name in validation) {
				description.validation = ((description.validation) ? (description.validation + " ") : "") + validation[description.name];
			}
			return description;
		});
	}

	getEndpoint(endpoint) {
		return this._makePath(endpoint);
	}

	async login(uid, password) {
		Exception.assert(this.isAuthentication(), "Authentication is not enabled.");
		return await this.options.authentication.login(this, uid, password);
	}

	async logout() {
		Exception.assert(this.isAuthentication(), "Authentication is not enabled.");
		return await this.options.authentication.logout(this);
	}

	async request(method, endpoint, data) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		// Build the options
		let fetchOptions = {
			method: method
		};
		if ("type" in responseOptions) {
			fetchOptions.expect = responseOptions.type;
		}

		if ("validation" in requestOptions) {
			Exception.assert(["json", "query"].includes(requestOptions.type), "{} {}: validation is not available for {}.", method, endpoint, requestOptions.type);
			const validation = new Validation(requestOptions.validation);
			validation.validate(data, {
				all: true
			});
		}

		switch (requestOptions.type) {
		case "json":
			Exception.assert(typeof data === "object", "Data must be of type 'object', got '{:j}' instead.", data);
			fetchOptions.data = data;
			break;
		case "query":
			Exception.assert(typeof data === "object", "Data must be of type 'object', got '{:j}' instead.", data);
			fetchOptions.query = data;
			break;
		}

		let retry;
		let retryCounter = 0;
		do {
			++retryCounter;
			retry = false;
    
			// Check if this is a request that needs authentication
			if (this.schema[endpoint][method].authentication) {
				Exception.assert(this.isAuthentication(), "This route has authentication requirement but no authentication object was specified.");
				await this.options.authentication.setAuthenticationFetch(fetchOptions);
			}

			try {
				const result = await Fetch.request(this._makePath(endpoint), fetchOptions);
				if ("validation" in responseOptions) {
					Exception.assert(["json"].includes(responseOptions.type), "{} {}: validation is not available for {}.", method, endpoint, responseOptions.type);
					const validation = new Validation(responseOptions.validation);
					validation.validate(result, {
						all: true
					});
				}
				return result;
			}
			catch (e) {
				if (e instanceof ExceptionFetch) {
					if (e.code == 401/*Unauthorized*/) {
						if (this.schema[endpoint][method].authentication) {
							await this.options.authentication.refreshAuthentication();
							retry = true;
							continue;
						}
					}
				}
				throw e;
			}
        
		} while (retry && retryCounter <= 1);
	}
}
