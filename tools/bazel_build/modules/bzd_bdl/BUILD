load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "defs.bzl",
])

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//extensions:json",
        "@bazel_skylib//lib:sets",
        "@bazel_skylib//rules:common_settings",
        "@bdl_extension//:extensions",
        "@bzd_lib//:sh_binary_wrapper",
        "@bzd_lib//config:defs",
        "@bzd_package//:defs",
        "@bzd_toolchain_cc//cc:defs",
    ],
)
