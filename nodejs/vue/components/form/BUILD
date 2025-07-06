load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/utils:pathlib",
    ],
)

bzd_nodejs_library(
    name = "form",
    srcs = glob([
        "**/*.vue",
    ]),
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:script",
        "//nodejs/core:upload",
        "//nodejs/core:validation",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/directives:touch",
    ],
)
