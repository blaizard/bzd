"use strict";

import Fetch from "../../../../nodejs/core/fetch.mjs";

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

export default {
    cache: [
        {
            collection: "github.builds",
            fetch: async (username, repository, workflowId, token) => {

                // Build the URL
                const baseUrl = "https://api.github.com/repos/" + encodeURIComponent(username) + "/" + encodeURIComponent(repository);
                const url = baseUrl + ((workflowId) ? ("/actions/workflows/" + encodeURIComponent(workflowId) + "/runs") : "/actions/runs");
                let options = {
                    expect: "json",
                    headers: {
                        "Content-Type": "application/vnd.github.v3+json",
                        "User-Agent": "curl"
                    }
                };

                if (token) {
                    options.authentication = {
                        type: "basic",
                        username: username,
                        password: token
                    };
                }

                const result = await Fetch.get(url, options);

                let builds = [];
                let promises = [];
                for (const item of result.workflow_runs || []) {
                    promises.push((async () => {
                        const resultJobs = await Fetch.get(item.jobs_url, options);
                        const job = resultJobs.jobs[0] || {};
                        const dateStart = Date.parse(job.started_at);
                        const dateEnd = Date.parse(job.completed_at);
                        builds.push({
                            duration: (dateEnd - dateStart) || 0,
                            timestamp: dateStart || Date.now(),
                            status: _getStatus(item),
                            link: item.html_url
                        });
                    })());
                }

                await Promise.all(promises);
                return builds;
            },
            timeout: 60 * 1000,
        }
    ],
    fetch: async (data, cache) => {
        const builds = await cache.get("github.builds", data["github.username"], data["github.repository"], data["github.workflowid"], data["github.token"]);
        return {
            builds: builds
        };
    }
}
