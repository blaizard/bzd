load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "utils",
    srcs = [
        "utils.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
    ],
)
