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

	return {
		data: await response.text(),
		headers: [...response.headers].reduce((obj, pair) => {
			obj[pair[0]] = pair[1];
			return obj;
		}, {}),
		code: response.status,
	};
}
