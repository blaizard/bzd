import { HttpClient } from "#bzd/nodejs/core/http/client.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Cache from "#bzd/nodejs/core/cache.js";

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
			async (key, context, { url, build, branch, user, token }) => {
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
			{ timeoutMs: 10 * 1000 },
		);
	}

	async fetch(cache) {
		const url = this.config["jenkins.url"];
		const build = this.config["jenkins.build"];
		const branch = this.config["jenkins.branch"] || "master";
		const user = this.config["jenkins.user"];
		const token = this.config["jenkins.token"];
		const builds = await cache.get("jenkins.builds", Cache.arrayOfStringToKey([url, build, branch, user]), {
			url: url,
			build: build,
			branch: branch,
			user: user,
			token: token,
		});
		return {
			builds: builds,
		};
	}
}
