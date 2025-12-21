load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "base",
    srcs = [
        "base.mjs",
    ],
    visibility = ["//nodejs/vue/components/terminal/backend:__subpackages__"],
    deps = [
        ":status",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "status",
    srcs = [
        "status.mjs",
    ],
    visibility = ["//visibility:public"],
)
