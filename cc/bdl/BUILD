load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "cc.bzl",
    "extension.bzl",
])

bzl_library(
    name = "cc",
    srcs = [
        "cc.bzl",
        "@rules_cc//cc:bzl_srcs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bazel_skylib//lib:sets",
        "@bzd_bdl//:defs",
        "@bzd_toolchain_cc//cc:defs",
    ],
)
