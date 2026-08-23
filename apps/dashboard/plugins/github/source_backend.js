import { HttpClient } from "#bzd/nodejs/core/http/client.js";
import Cache2 from "#bzd/nodejs/core/cache2.js";

function _getStatus(item) {
	if (item.conclusion == "success") {
		return "success";
	}
	if (["cancelled", "skipped"].indexOf(item.conclusion) != -1) {
		return "abort";
	}
	if (item.conclusion) {
		return "failure";
	}
	if (["queued", "in_progress"].indexOf(item.status) != -1) {
		return "in-progress";
	}
	return "unknown";
}

function getFetchOptions(username, token) {
	let options = {
		expect: "json",
		headers: {
			"Content-Type": "application/vnd.github.v3+json",
			"User-Agent": "curl",
		},
	};

	if (token) {
		options.authentication = {
			type: "basic",
			username: username,
			password: token,
		};
	}

	return options;
}

export default class Github {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		cache.register(
			"github.builds",
			async (key, context, { username, repository, workflowId, token }) => {
				// Build the URL
				const baseUrl =
					"https://api.github.com/repos/" + encodeURIComponent(username) + "/" + encodeURIComponent(repository);
				const url =
					baseUrl + (workflowId ? "/actions/workflows/" + encodeURIComponent(workflowId) + "/runs" : "/actions/runs");

				const result = await HttpClient.get(url, getFetchOptions(username, token));

				let builds = [];
				let promises = [];
				for (const item of result.workflow_runs || []) {
					promises.push(
						(async () => {
							const result = await cache.get("github.jobs", item.jobs_url, { username, token });
							if ("status" in result) {
								builds.push(result);
							}
						})(),
					);
				}

				await Promise.all(promises);
				return builds;
			},
			{ timeoutMs: 60 * 1000 },
		);

		cache.register("github.jobs", async (key, context, { username, token }) => {
			const resultJobs = await HttpClient.get(key, getFetchOptions(username, token));

			// Filter out empty jobs
			if (resultJobs.jobs.length == 0) {
				return {};
			}

			let dateStart = Date.now();
			let dateEnd = 0;
			let status = "unknown";
			let link = null;
			for (const job of resultJobs.jobs) {
				dateStart = Math.min(dateStart, Date.parse(job.started_at) || Date.now());
				dateEnd = Math.max(dateEnd, Date.parse(job.completed_at) || 0);
				const jobStatus = _getStatus(job);
				if (jobStatus == "in-progress") {
					status = jobStatus;
				} else if (jobStatus == "failure") {
					status = jobStatus;
				} else if (jobStatus == "success" && status == "unknown") {
					status = jobStatus;
				} else if (jobStatus == "abort" && ["unknown", "success"].indexOf(status) != -1) {
					status = jobStatus;
				}
				link = job.html_url;
			}

			if (status == "in-progress") {
				context.timeoutMs = 60 * 1000; // Refresh every minutes
			}

			return {
				duration: dateEnd - dateStart || 0,
				timestamp: dateStart || Date.now(),
				status: status,
				link: link,
			};
		});
	}

	async fetch(cache) {
		const username = this.config["github.username"];
		const repository = this.config["github.repository"];
		const workflowId = this.config["github.workflowid"];
		const token = this.config["github.token"];
		const builds = await cache.get(
			"github.builds",
			Cache2.arrayOfStringToKey([username, repository, workflowId]),
			{ username: username, repository: repository, workflowId: workflowId, token: token },
		);
		return {
			builds: builds,
		};
	}
}
