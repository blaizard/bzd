load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "docker.bzl",
    "cc.bzl",
    "bdl.bzl",
])

bzl_library(
    name = "docker",
    srcs = [
        "docker.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@io_bazel_rules_docker//container",
    ],
)

bzl_library(
    name = "cc",
    srcs = [
        "cc.bzl",
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
        "//tools/bazel_build/rules/third_party/cc:bzl",
        "@bazel_skylib//lib:sets",
        "@bazel_skylib//rules:common_settings",
        "@bzd_package//:package",
        "@bzd_toolchain_cc//cc:defs",
        "@bzd_utils//:sh_binary_wrapper",
    ],
)
