"use strict";

import { request as requestHttp } from "http";
import { request as requestHttps } from "https";
import ExceptionFactory from "../../exception.js";
import LogFactory from "../../log.js";

const Exception = ExceptionFactory("fetch", "node.http");
const Log = LogFactory("fetch", "node.http");

const MAX_REDIRECTION = 3;

export default async function request(url, options) {

    return new Promise((resolve, reject) => {

        // Check if http or https
        let requestHandler = (url.toLowerCase().startsWith("https://")) ? requestHttps : requestHttp;

        // Create the request
        let req = requestHandler(url, {
            method: options.method,
            headers: options.headers
        }, (response) => {

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
                    return request(response.headers.location, options).then(resolve).catch(reject);
                }
            }

            if (response.statusCode < 200 || response.statusCode > 299) {
                return reject(new Exception("Request to '{}' responded with: {} {}", url, response.statusCode, response.statusMessage));
            }

            console.log("HELLO");

            response.setEncoding("utf8");
            let body = "";
            response.on("data", (chunk) => {
                body += chunk;
            });
            response.on("end", () => {
                resolve(body);
            });
        });

        if (typeof options.body !== "undefined") {
            Exception.assert(typeof options.body === "string", "Body passed must be a string: {}", options.body);
            req.setHeader("Content-Length", Buffer.byteLength(options.body));
            req.write(options.body);
        }

        req.on("error", (e) => { reject(e) });
        req.end();
    });
}
