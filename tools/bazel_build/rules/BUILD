load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "docker.bzl",
    "cc.bzl",
    "bdl.bzl",
])

bzl_library(
    name = "cc",
    srcs = [
        "cc.bzl",
        "@rules_cc//cc:bzl_srcs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "@bazel_skylib//lib:sets",
        "@bzd_toolchain_cc//cc:defs",
    ],
)

bzl_library(
    name = "bdl",
    srcs = [
        "bdl.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//tools/bazel_build/rules/extensions:cc.bzl",
        "@bazel_skylib//lib:sets",
        "@bazel_skylib//rules:common_settings",
        "@bzd_lib//:sh_binary_wrapper",
        "@bzd_package//:defs",
        "@bzd_toolchain_cc//cc:defs",
    ],
)
