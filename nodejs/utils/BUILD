load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "color",
    srcs = [
        "color.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "delay",
    srcs = [
        "delay.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "object",
    srcs = [
        "object.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "query",
    srcs = [
        "query.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "pathlib",
    srcs = [
        "pathlib.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "to_string",
    srcs = [
        "to_string.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:format",
    ],
)

bzd_nodejs_library(
    name = "uid",
    srcs = [
        "uid.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "regexpr",
    srcs = [
        "regexpr.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "result",
    srcs = [
        "result.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:format",
    ],
)
