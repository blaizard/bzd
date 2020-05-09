"use strict";

import ExceptionFactory from "../../exception.mjs";

const Exception = ExceptionFactory("fetch", "window.fetch");

export default async function request(url, options) {

	// Add support for FormData
	if (options.body instanceof FormData) {
		options.headers["Content-Type"] = "application/x-www-form-urlencoded";
		options.body = JSON.stringify(options.body);
	}

	const response = await window.fetch(url, {
		method: options.method,
		body: options.body,
		headers: options.headers
	});

	if (!response.ok) {
		const message = await response.text();
		throw new Exception(message || response.statusText);
	}

	return await response.text();
}
