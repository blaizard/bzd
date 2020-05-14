"use strict";

import Base from "./base.mjs";
import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("api", "server");

export default class APIServer extends Base {

    /**
     * Register a callback to handle a request
     */
	handle(web, method, endpoint, callback, /*options = {}*/) {
		this._sanityCheck(method, endpoint);
		const requestOptions = this.schema[endpoint][method].request || {};
		const responseOptions = this.schema[endpoint][method].response || {};

		// Build the web options
		let webOptions = {};
		if ("type" in requestOptions) {
			webOptions.type = [requestOptions.type];
		}

		// Create a wrapper to the callback
		const callbackWrapper = async function(request, response) {
			try {
				let data = null;
				switch (requestOptions.type) {
				case "json":
					data = request.body.data;
					break;
				case "query":
					data = request.query;
					break;
				case "upload":
					data = Object.assign({}, request.query || {}, {
						files: Object.keys(request.files || {}).map((key) => request.files[key].path)
					});
					break;
				}

				const result = await callback.call(this, data);
				switch (responseOptions.type) {
				case "json":
					Exception.assert(typeof result == "object", "{} {}: callback result must be a json object.", method, endpoint);
					response.json(result);
					break;
				case "file":
					if (typeof result == "string") {
						response.sendFile(result);
					}
					else if ("pipe" in result) {
						await (new Promise((resolve, reject) => {
							result.on("error", reject)
								.on("end", resolve)
								.on("finish", resolve)
								.pipe(response);
						}));
						response.end();
					}
					else {
						Exception.unreachable("{} {}: callback result is not of a supported format.", method, endpoint);
					}
					break;
				case "raw":
					response.status(200).send(result);
					break;
				default:
					response.sendStatus(200);
				}
			}
			catch (e) {
				Exception.print("Exception Guard");
				Exception.print(Exception.fromError(e));
				response.status(500).send(e.message);
			}
		};

		web.addRoute(method, this._makePath(endpoint), callbackWrapper, webOptions);
	}
}
