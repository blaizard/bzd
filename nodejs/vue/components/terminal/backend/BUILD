load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "status",
    srcs = [
        "status.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "command",
    srcs = [
        "command.mjs",
    ],
    tools = [
        "//nodejs/vue/components/terminal/backend/bin:terminal",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":status",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
