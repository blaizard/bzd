load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "continuous-integration",
    srcs = glob([
        "*",
    ]),
    visibility = [
        "//apps/dashboard/plugins:__pkg__",
    ],
    deps = [
        "//nodejs/vue/components/graph",
    ],
)
