"use strict";

import Fetch from "../../../../nodejs/core/fetch.js";

function _getStatus(item) {
    if (item.result == "SUCCESS") {
        return "success";
    }
    if (item.result == "FAILURE") {
        return "failure";
    }
    if (item.result == "ABORTED") {
        return "aborted";
    }
    if (item.result == null) {
        return "processing";
    }
    return "unknown";
}

export default {
    fetch: async (data) => {

        // Build the URL
        const baseUrl = data["jenkins.url"] + "/job/" + data["jenkins.build"] + "/job/" + (data["jenkins.branch"] || "master");
        let options = {
            expect: "json"
        };

        // If authentication
        if (data["jenkins.user"]) {
            options.authentication = {
                type: "basic",
                username: data["jenkins.user"],
                password: data["jenkins.token"]
            };
        }

        const result = await Fetch.get(baseUrl + "/api/json?&tree=builds[duration,result,id,timestamp,estimatedDuration]{0,30}", options);
        
        const builds = (result.builds || []).map((item) => {
            const status = _getStatus(item);
            return {
                duration: (status == "processing") ? item.estimatedDuration : item.duration,
                timestamp: item.timestamp,
                status: status,
                link: baseUrl + "/" + item.id
            }
        })

        return {
            builds: builds
        };
    }
}
