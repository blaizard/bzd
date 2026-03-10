load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "local",
    srcs = [
        "command.mjs",
    ],
    tools = [
        "//nodejs/vue/components/terminal/backend/local/bin:terminal",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/utils:run",
        "//nodejs/vue/components/terminal/backend:base",
    ],
)
