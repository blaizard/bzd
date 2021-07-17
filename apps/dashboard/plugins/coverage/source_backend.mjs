import HttpClient from "bzd/core/http/client.mjs";

export default {
	cache: [
		{
			collection: "coverage.report",
			fetch: async (reportUrl, authentication) => {
				return await HttpClient.get(reportUrl, {
					expect: "json",
					authentication: authentication
				});
			},
			timeout: 60 * 1000
		}
	],
	fetch: async (data, cache) => {
		const authentication = {
			type: data["coverage.authentication"],
			username: data["coverage.user"],
			password: data["coverage.token"]
		};
		return await cache.get("coverage.report", data["coverage.url"], authentication);
	}
};
