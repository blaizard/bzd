load("//tools/bazel_build/rules:nodejs_web.bzl", "bzd_nodejs_web")
load("//tools/bazel_build/rules:package.bzl", "bzd_package")

bzd_nodejs_web(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]),
    entries = {
        "app.mjs": "index",
    },
    visibility = [
        "//apps/artifacts:__pkg__",
    ],
    deps = [
        "//apps/artifacts:api",
        "//nodejs:vue",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)

bzd_package(
    name = "local",
    args = ["/tmp/bzd"],
    tags = ["nodejs"],
    deps = {
        ":frontend": "frontend",
    },
)
