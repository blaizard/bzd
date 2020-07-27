load(":defs.bzl", "bzd_setting_cache")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
)

bzd_setting_cache(
    name = "cache",
    build_setting_default = "",
    visibility = ["//visibility:public"],
)
