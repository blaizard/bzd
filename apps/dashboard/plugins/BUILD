load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "plugins",
    srcs = [
        "plugins.js",
    ],
    visibility = [
        "//apps/dashboard:__subpackages__",
    ],
    deps = [
        "//apps/dashboard/plugins/continuous-integration",
        "//apps/dashboard/plugins/jenkins",
        "//apps/dashboard/plugins/link",
        "//apps/dashboard/plugins/travisci",
    ],
)
