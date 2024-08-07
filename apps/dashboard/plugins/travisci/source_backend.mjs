import HttpClient from "#bzd/nodejs/core/http/client.mjs";

function _getStatus(item) {
	if (item.state == "passed") {
		return "success";
	}
	if (item.state == "failed" || item.state == "errored") {
		return "failure";
	}
	if (item.state == "created" || item.state == "started") {
		return "in-progress";
	}
	if (item.state == "canceled") {
		return "abort";
	}
	return "unknown";
}

export default class TravisCI {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		cache.register(
			"travisci.builds",
			async (endpoint, repositorySlug, token) => {
				// Build the URL
				const url = "https://api." + endpoint + "/repo/" + encodeURIComponent(repositorySlug) + "/builds?limit=50";
				let options = {
					expect: "json",
					headers: {
						"Travis-API-Version": "3",
					},
					authentication: {
						type: "token",
						token: token,
					},
				};

				const result = await HttpClient.get(url, options);
				return (result.builds || []).map((item) => {
					const status = _getStatus(item);
					return {
						// Duration attribute is not correct as it includes accumulated duration from the jobs even though they
						// run in parallel. So we use the actual run time instead.
						duration: Date.parse(item.finished_at) - Date.parse(item.started_at),
						timestamp:
							Date.parse(item.started_at) || Date.parse(item.finished_at) || Date.parse(item.updated_at) || Date.now(),
						status: status,
						link: "https://app." + endpoint + "/" + repositorySlug + "/builds/" + item.id,
					};
				});
			},
			{ timeout: 10 * 1000 },
		);
	}

	async fetch(cache) {
		const builds = await cache.get(
			"travisci.builds",
			this.config["travisci.endpoint"],
			this.config["travisci.repository"],
			this.config["travisci.token"],
		);
		return {
			builds: builds,
		};
	}
}
