import ExceptionFactory from "../exception.mjs";
import HttpClient from "../http/client.mjs";
import { HttpClientException } from "../http/client.mjs";
import Validation from "../validation.mjs";

import Base from "./base.mjs";

const Exception = ExceptionFactory("api", "client");

export default class APIClient extends Base {
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

		let retry;
		let retryCounter = 0;
		do {
			++retryCounter;
			retry = false;

			// Check if this is a request that needs authentication
			if (this.schema[endpoint][method].authentication) {
				Exception.assert(
					this.isAuthentication(),
					"This route has authentication requirement but no authentication object was specified.",
				);
				await this.options.authentication.setAuthenticationFetch(fetchOptions);
			}

			try {
				const result = await HttpClient.request(this.getEndpoint(endpoint), fetchOptions);
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
			catch (e) {
				if (e instanceof HttpClientException) {
					if (e.code == 401 /*Unauthorized*/) {
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
