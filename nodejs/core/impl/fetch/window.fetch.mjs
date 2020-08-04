import { FetchException } from "../../fetch.mjs";

export default async function request(url, options) {
	// Add support for FormData
	if (options.body instanceof FormData) {
		options.headers["Content-Type"] = "application/x-www-form-urlencoded";
		options.body = JSON.stringify(options.body);
	}

	const response = await window.fetch(url, {
		method: options.method,
		body: options.body,
		headers: options.headers,
	});

	if (!response.ok) {
		const message = await response.text();
		throw new FetchException(response.status, message || response.statusText);
	}

	return [
		await response.text(),
		[...response.headers].reduce((obj, pair) => {
			obj[pair[0]] = pair[1];
			return obj;
		}, {}),
	];
}
