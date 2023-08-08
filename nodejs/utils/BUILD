load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "color",
    srcs = [
        "color.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
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
        "//nodejs:bzd",
        "//nodejs/core:format",
    ],
)

bzd_nodejs_library(
    name = "uid",
    srcs = [
        "uid.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
    ],
)

bzd_nodejs_library(
    name = "regexpr",
    srcs = [
        "regexpr.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
    ],
)
