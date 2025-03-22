load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "influxdb",
    srcs = [
        "influxdb.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":sink",
        "//nodejs/core/http:client",
    ],
)

bzd_nodejs_library(
    name = "sink",
    srcs = [
        "sink.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/services",
    ],
)
