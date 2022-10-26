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
    packages = {
        "highlight.js": "",
    },
    visibility = [
        "//apps/player:__pkg__",
    ],
    deps = [
        "//apps/player:api",
        "//apps/player/lib:filesystem",
        "//apps/player/lib:scenario",
        "//nodejs:vue",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/router",
    ],
)

bzd_package(
    name = "local",
    args = ["/tmp/bzd"],
    include_metadata = True,
    tags = ["nodejs"],
    deps = {
        ":frontend": "frontend",
    },
)
