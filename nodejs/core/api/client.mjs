"use strict";

import Base from "./base.mjs";
import Fetch from "../fetch.mjs";
import ExceptionFactory from "../exception.mjs";
import ExceptionFetch from "../impl/fetch/exception.mjs";

const Exception = ExceptionFactory("api", "client");

export default class APIClient extends Base {

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
        do {
            retry = false;
    
            // Check if this is a request that needs authentication
            if (this.schema[endpoint][method].authentication) {
                Exception.assert(this.options.authentication, "This route has authentication requirement but no authentication object was specified.");
                await this.options.authentication.assertAuthentication();

                // Automatically add authentication information to the request
                fetchOptions.authentication = {
                    type: "token",
                    token: this.options.authentication.getToken()
                }
            }

            try {
                return await Fetch.request(this._makePath(endpoint), fetchOptions);
            }
            catch (e) {
                if (e instanceof ExceptionFetch) {
                    if (e.code == 401/*Unauthorized*/ || e.code == 403/*Forbidden*/) {
                        if (this.options.authentication) {
                            await this.options.authentication.tryRefreshToken();
                            retry = true;
                            continue;
                        }
                    }
                }
                throw e;
            }
        
        } while (retry);
	}
}
