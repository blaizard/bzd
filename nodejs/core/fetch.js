"use strict";

/**
 * Fetch data from HTTP endpoint.
 */
export default class Fetch {
    static async _process(url, options = {}) {

        // Handle queries
        if ("query" in options) {
            const query = Object.keys(options.query).map((key) => key + "=" + encodeURIComponent(options.query[key])).join("&");
            url += ((query) ? ("?" + query) : "");
        }

        let headers = options.headers || {};
        let body = undefined;

        // Generate body
        if (options.data instanceof FormData) {
            headers["Content-Type"] = "application/x-www-form-urlencoded";
            body = JSON.stringify(options.data);
        }
        else if (typeof options.data == "object") {
            headers["Content-Type"] = "application/json";
            body = JSON.stringify(options.data);
        }
        else {
            body = options.data;
        }

        // Handle expected type
        switch (options.expect) {
        case "json":
            headers["Accept"] = "application/json";
            break;
        }

        const response = await window.fetch(url, {
            method: options.method || "get",
            body: body,
            headers: headers
        });

        if (!response.ok) {
            throw Error((await response.text()) || response.statusText);
        }

        switch (options.expect) {
        case "json":
            return await response.json();
        default:
            return await response.text();
        }
    }

    static async get(url, options = {}) {
        options["method"] = "get";
        return await this._process(url, options);
    }
};
