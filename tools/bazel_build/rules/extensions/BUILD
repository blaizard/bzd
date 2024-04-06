load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "cc.bzl",
    "json.bzl",
])

bzl_library(
    name = "cc",
    srcs = [
        "cc.bzl",
    ],
    visibility = ["//tools/bazel_build/rules:__pkg__"],
    deps = [
        "@bzd_toolchain_cc//cc:defs",
    ],
)

bzl_library(
    name = "json",
    srcs = [
        "json.bzl",
    ],
    visibility = ["//tools/bazel_build/rules:__pkg__"],
)
