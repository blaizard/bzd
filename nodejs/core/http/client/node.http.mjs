import { request as requestHttp } from "http";
import { request as requestHttps } from "https";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";

const Exception = ExceptionFactory("http", "client", "node.http");
const Log = LogFactory("http", "client", "node.http");

const MAX_REDIRECTION = 3;

export default async function request(url, options) {
	return new Promise((resolve, reject) => {
		// Check if http or https
		let requestHandler = url.toLowerCase().startsWith("https://") ? requestHttps : requestHttp;

		// Doing this will help to have a clean stack trace
		const exceptionTimeout = new Exception("Request timeout (" + options.timeoutMs + " ms)");

		// Create the request
		let req = requestHandler(
			url,
			{
				method: options.method,
				headers: options.headers,
				timeout: options.timeoutMs
			},
			(response) => {
				// Handle redirection
				if (response.statusCode >= 300 && response.statusCode < 400) {
					// Limit the number of redirections
					options.redirection || (options.redirection = 0);
					if (options.redirection >= MAX_REDIRECTION) {
						return reject(new Exception("Too many redirection ({}) when requesting '{}'", options.redirection, url));
					}
					++options.redirection;

					// Redirect
					if ("location" in response.headers) {
						Log.debug("Redirecting to '{}'", response.headers.location);
						return request(response.headers.location, options)
							.then(resolve)
							.catch(reject);
					}
				}

				let result = {
					data: null,
					headers: response.headers,
					code: response.statusCode
				};

				if (options.expect == "stream") {
					result.data = response;
					resolve(result);
				} else {
					let data = "";
					response.on("data", (chunk) => {
						data += chunk;
					});
					response.on("end", () => {
						result.data = data;
						resolve(result);
					});
				}
			}
		);

		// A string
		if (typeof options.data == "string") {
			req.write(options.data);
			req.end();
		}
		// A read stream
		else if (typeof options.data == "object" && typeof options.data.pipe == "function") {
			options.data.pipe(req, { end: true });
		} else if (typeof options.data == "undefined") {
			req.end();
		} else {
			reject(new Exception("Unsupported data format: {:j}", options.data));
		}

		req.on("timeout", () => {
			req.abort();
			reject(exceptionTimeout);
		});

		req.on("error", (e) => {
			reject(e);
		});
	});
}
