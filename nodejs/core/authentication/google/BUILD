load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
        "google.svg",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:cookie",
        "//nodejs/core:script",
        "//nodejs/utils:query",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    apis = ["api.json"],
    packages = {
        "google-auth-library": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:script",
    ],
)
