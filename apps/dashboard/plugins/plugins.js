"use strict";

export const Source = {
    jenkins: {
        timeout: 10 * 1000, // ms
        frontend: () => import("./jenkins/source_frontend.vue"),
        backend: () => import("./jenkins/source_backend.js")
    }
};

export const Visualization = {
    "continuous-integration": {
        frontend: () => import("./continuous-integration/visualization.vue")
    }
}
