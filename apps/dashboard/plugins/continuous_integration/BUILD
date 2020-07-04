load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "continuous_integration",
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
