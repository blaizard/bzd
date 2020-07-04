load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "plugins",
    srcs = [
        "plugins.mjs",
    ],
    visibility = [
        "//apps/dashboard:__subpackages__",
    ],
    deps = [
        "//apps/dashboard/plugins/continuous_integration",
        "//apps/dashboard/plugins/coverage",
        "//apps/dashboard/plugins/github",
        "//apps/dashboard/plugins/jenkins",
        "//apps/dashboard/plugins/link",
        "//apps/dashboard/plugins/travisci",
    ],
)
