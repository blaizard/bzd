load("//tools/bazel.build/rules:nodejs_web.bzl", "bzd_nodejs_web")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")

bzd_nodejs_web(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.mjs",
    ]),
    entries = {
        "app.mjs": "index",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//apps/dashboard:api",
        "//apps/dashboard/plugins",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:api",
        "//nodejs/core:fetch",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)

bzd_package(
    name = "local",
    args = ["/tmp"],
    tags = ["nodejs"],
    deps = {
        ":frontend": "frontend",
    },
)
