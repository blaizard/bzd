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
        "vue-router": "",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//nodejs:icons",
        "//nodejs:styles",
        "//nodejs:vue",
    ],
)
