load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "websocket",
    srcs = [
        "vt100.vue",
        "websocket.vue",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
