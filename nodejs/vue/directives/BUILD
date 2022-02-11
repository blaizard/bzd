load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "touch",
    srcs = [
        "touch.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
    ],
)

bzd_nodejs_library(
    name = "tooltip",
    srcs = [
        "tooltip.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "resize",
    srcs = [
        "resize.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
    ],
)

bzd_nodejs_library(
    name = "mask",
    srcs = [
        "mask.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
    ],
)

bzd_nodejs_library(
    name = "loading",
    srcs = [
        "loading.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":mask",
    ],
)
