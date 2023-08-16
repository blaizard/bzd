import HttpClient from "bzd/core/http/client.mjs";

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

export default {
	cache: {
		"jenkins.builds": {
			fetch: async (url, build, branch, user, token) => {
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

				const result = await HttpClient.get(
					baseUrl + "/api/json?&tree=builds[duration,result,id,timestamp,estimatedDuration]{0,100}",
					options
				);

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
			timeout: 10 * 1000,
		},
	},
	fetch: async (data, cache) => {
		const builds = await cache.get(
			"jenkins.builds",
			data["jenkins.url"],
			data["jenkins.build"],
			data["jenkins.branch"] || "master",
			data["jenkins.user"],
			data["jenkins.token"]
		);
		return {
			builds: builds,
		};
	},
};
