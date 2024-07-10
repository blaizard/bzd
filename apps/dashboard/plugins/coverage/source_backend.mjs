import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("plugin", "coverage");

export default class Coverage {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		cache.register(
			"coverage.report",
			async (reportUrl, authentication) => {
				try {
					return await HttpClient.get(reportUrl, {
						expect: "json",
						authentication: authentication,
					});
				} catch (e) {
					Exception.errorPrecondition(e);
				}
			},
			{ timeout: 60 * 1000 },
		);
	}

	async fetch(cache) {
		const authentication = {
			type: this.config["coverage.authentication"],
			username: this.config["coverage.user"],
			password: this.config["coverage.token"],
		};
		return await cache.get("coverage.report", this.config["coverage.url"], authentication);
	}
}
