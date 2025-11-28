load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "clock",
    srcs = [
        "clock.mjs",
    ],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "date",
    srcs = [
        "date.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":clock",
    ],
)

bzd_nodejs_library(
    name = "mock",
    srcs = [
        "mock.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":clock",
    ],
)

bzd_nodejs_library(
    name = "ntp",
    srcs = [
        "ntp.mjs",
    ],
    packages = [
        "@nodejs_deps//:ntp-client",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":clock",
    ],
)
