"use strict";

export const Frontend = {
    jenkins: () => import("./jenkins/jenkins.vue")
};

export const Backend = {
    jenkins: () => import("./jenkins/backend.js")
};
