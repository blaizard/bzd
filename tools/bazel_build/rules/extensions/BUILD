load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "cc.bzl",
])

bzl_library(
    name = "cc",
    srcs = [
        "cc.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_toolchain_cc//cc:defs",
    ],
)
