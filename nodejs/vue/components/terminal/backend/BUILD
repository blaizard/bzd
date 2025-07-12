load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "terminal",
    srcs = [
        "terminal.mjs",
    ],
    tools = [
        "//nodejs/vue/components/terminal/backend/bin:terminal",
    ],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "command",
    srcs = [
        "command.mjs",
    ],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "websocket",
    srcs = [
        "websocket.mjs",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":command",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
