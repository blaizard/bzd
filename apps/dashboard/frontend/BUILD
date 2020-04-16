load("//tools/bazel.build/rules:nodejs_web.bzl", "bzd_nodejs_web")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")

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
        "//nodejs/core:fetch",
    ],
)

bzd_package(
    name = "dev",
    tags = ["nodejs"],
    args = ["/tmp"],
    deps = {
        ":frontend": "frontend",
    },
)
