"use strict";

import Fetch from "../../../../nodejs/core/fetch.js";

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

        const result = await Fetch.get(baseUrl + "/api/json?&tree=builds[duration,result,id]{0,10}", options);

        const builds = (result.builds || []).map((item) => {
            return {
                duration: item.duration,
                status: item.result,
                link: baseUrl + "/" + item.id
            }
        })

        console.log(builds);

        return {
            builds: builds
        };
    }
}
