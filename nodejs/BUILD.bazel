load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "vue",
    packages = [
        "@nodejs_deps//:vue",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "icons",
    srcs = [
        "icons.scss",
    ],
    visibility = ["//visibility:public"],
)
