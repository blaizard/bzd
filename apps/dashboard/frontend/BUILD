load("//tools/bazel.build/rules:nodejs_web.bzl", "bzd_nodejs_web")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")

bzd_nodejs_web(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.js",
        "**/*.svg",
    ]),
    alias = "frontend",
    entries = {
        "app.js": "index",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//apps/dashboard:api",
        "//nodejs:icons",
        "//nodejs:styles",
        "//nodejs:vue",
        "//nodejs/vue/router",
        "//nodejs/core:fetch",
        "//nodejs/core:api",
        "//nodejs/vue:api",
        "//nodejs/vue/components:form",
        "//nodejs/vue/directives:tooltip",
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
