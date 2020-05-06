"use strict";

export default {
    source: {
        name: "Jenkins",
        icon: "bzd-icon-jenkins",
        visualization: ["continuous-integration"],
        form: [
            { type: "Input", name: "jenkins.url", caption: "Jenkins URL", placeholder: "http://localhost:8080", width: 1 },
            { type: "Input", name: "jenkins.user", caption: "User", width: 0.5 },
            { type: "Input", name: "jenkins.token", caption: "Token", width: 0.5 },
            { type: "Input", name: "jenkins.build", caption: "Build name", width: 0.5 },
            { type: "Dropdown", name: "jenkins.branch", caption: "Branch", placeholder: "master", list: ["master"], editable: true, width: 0.5 },
        ],
        defaultValue: {
            "jenkins.branch": "master"
        },
        timeout: 60 * 1000, // ms
        frontend: () => import("./source_frontend.vue"),
        backend: () => import("./source_backend.js")
    }
};
