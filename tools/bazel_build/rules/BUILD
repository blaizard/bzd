load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

exports_files([
    "nodejs.bzl",
    "nodejs_web.bzl",
    "package.bzl",
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

bzd_nodejs_library(
    name = "mocha",
    packages = {
        "c8": "",
        "mocha": "",
    },
    visibility = ["//visibility:public"],
)

bzl_library(
    name = "nodejs",
    srcs = [
        "nodejs.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":package",
        "@bazel_skylib//rules:common_settings",
        "@bzd_utils//:sh_binary_wrapper",
        "@io_bazel_rules_docker//container",
    ],
)

bzl_library(
    name = "nodejs_web",
    srcs = [
        "nodejs_web.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":nodejs",
    ],
)

bzl_library(
    name = "package",
    srcs = [
        "package.bzl",
    ],
    visibility = ["//visibility:public"],
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
        ":package",
        "//tools/bazel_build/rules/third_party/cc:bzl",
        "@bazel_skylib//lib:sets",
        "@bazel_skylib//rules:common_settings",
        "@bzd_toolchain_cc//cc:defs",
        "@bzd_utils//:sh_binary_wrapper",
    ],
)
