load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "docker",
    srcs = [
        "command.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/utils:run",
        "//nodejs/vue/components/terminal/backend:base",
    ],
)
