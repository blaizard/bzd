load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "base",
    srcs = [
        "base.mjs",
    ],
    visibility = ["//nodejs/vue/components/terminal/backend:__subpackages__"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/utils:run",
    ],
)
