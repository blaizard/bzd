"use strict";

export default {
    source: {
        timeout: 10 * 60 * 1000, // ms
        frontend: () => import("./source_frontend.vue"),
        backend: () => import("./source_backend.js")
    }
};
