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
        "vue": "",
    },
    deps = [
        "//platform/nodejs:icons",
        "//platform/nodejs:styles",
    ],
)
