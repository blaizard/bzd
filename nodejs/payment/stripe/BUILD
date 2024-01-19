load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "webhook",
    srcs = [
        "webhook.mjs",
    ],
    apis = ["api.json"],
    packages = {
        "stripe": "14.12.0",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
    ],
)
