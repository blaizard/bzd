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
        "@bzd_bdl//:defs",
        "@bazel_skylib//lib:sets",
        "@bzd_toolchain_cc//cc:defs",
    ],
)
