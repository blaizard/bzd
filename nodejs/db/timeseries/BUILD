load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.mjs",
        "timeseries.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/persistence:timeseries",
        "//nodejs/db:utils",
    ],
)

bzd_nodejs_library(
    name = "elasticsearch",
    srcs = [
        "elasticsearch.mjs",
        "timeseries.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/db:utils",
    ],
)
