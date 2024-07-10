import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("plugin", "jenkins");

function _getStatus(item) {
	if (item.result == "SUCCESS") {
		return "success";
	}
	if (item.result == "FAILURE") {
		return "failure";
	}
	if (item.result == "ABORTED") {
		return "abort";
	}
	if (item.result == null) {
		return "in-progress";
	}
	return "unknown";
}

export default class Jenkins {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		cache.register(
			"jenkins.builds",
			async (url, build, branch, user, token) => {
				// Build the URL
				const baseUrl = url + "/job/" + build + "/job/" + branch;
				let options = {
					expect: "json",
				};

				// If authentication
				if (user) {
					options.authentication = {
						type: "basic",
						username: user,
						password: token,
					};
				}

				let result = null;
				try {
					result = await HttpClient.get(
						baseUrl + "/api/json?&tree=builds[duration,result,id,timestamp,estimatedDuration]{0,100}",
						options,
					);
				} catch (e) {
					Exception.errorPrecondition(e);
				}

				return (result.builds || []).map((item) => {
					const status = _getStatus(item);
					return {
						duration: status == "processing" ? item.estimatedDuration : item.duration,
						timestamp: item.timestamp,
						status: status,
						link: baseUrl + "/" + item.id,
					};
				});
			},
			{ timeout: 10 * 1000 },
		);
	}

	async fetch(cache) {
		const builds = await cache.get(
			"jenkins.builds",
			this.config["jenkins.url"],
			this.config["jenkins.build"],
			this.config["jenkins.branch"] || "master",
			this.config["jenkins.user"],
			this.config["jenkins.token"],
		);
		return {
			builds: builds,
		};
	}
}
