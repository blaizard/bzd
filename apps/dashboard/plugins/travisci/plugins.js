"use strict";

export default {
    source: {
        name: "Travis CI",
        icon: "bzd-icon-travisci",
        visualization: ["continuous-integration"],
        form: [
            { type: "Input", name: "travisci.repository", caption: "Repository", placeholder: "For example, blaizard/cpp-async", width: 0.5 },
            { type: "Input", name: "travisci.token", caption: "Token", width: 0.5 },
        ],
        timeout: 60 * 1000, // ms
        frontend: () => import("./source_frontend.vue"),
        backend: () => import("./source_backend.js")
    }
};
