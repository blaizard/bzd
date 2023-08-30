load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "filter",
    srcs = [
        "filter.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "workspace",
    srcs = [
        "workspace.mjs",
    ],
    data = [
        "//tools/sanitizer:config",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":filter",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
    ],
)
