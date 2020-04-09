load("//tools/bazel.build/rules:nodejs_web.bzl", "bzd_nodejs_web")

bzd_nodejs_web(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.js",
    ]),
    alias = "frontend",
    entries = {
        "app.js": "index",
    },
    packages = {
        "vue-router": ""
    },
    deps = [
        "//platform/nodejs:vue",
        "//platform/nodejs:icons",
        "//platform/nodejs:styles",
    ],
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
)
