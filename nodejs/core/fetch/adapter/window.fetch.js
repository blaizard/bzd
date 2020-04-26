"use strict";

import ExceptionFactory from "../../exception.js";

const Exception = ExceptionFactory("fetch", "window.fetch");

export default async function request(url, method, headers, body, options) {

	// Add support for FormData
	if (body instanceof FormData) {
		headers["Content-Type"] = "application/x-www-form-urlencoded";
		body = JSON.stringify(body);
	}

	const response = await window.fetch(url, {
		method: method,
		body: body,
		headers: headers
	});

	Exception.assert(response.ok, async () => { return (await response.text()) || response.statusText; });

	switch (options.expect) {
	case "json":
		return await response.json();
	default:
		return await response.text();
	}
}
