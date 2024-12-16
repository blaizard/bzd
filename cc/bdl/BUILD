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
        "@@protobuf+//bazel:cc_proto_library_bzl",
        "@bazel_skylib//lib:sets",
        "@bzd_bdl//:defs",
        "@bzd_toolchain_cc//cc:defs",
        "@rules_cc//cc:bzl_srcs",
        "@rules_cc//cc:find_cc_toolchain.bzl",
        "@rules_cc//cc/common",
        "@rules_cc//cc/toolchains:toolchain_rules",
    ],
)
