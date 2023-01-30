load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "nodejs.bzl",
    "nodejs_web.bzl",
    "package.bzl",
    "docker.bzl",
    "cc.bzl",
    "bdl.bzl",
    "documentation.bzl",
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
    name = "documentation",
    srcs = [
        "documentation.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//tools/bazel_build:binary_wrapper",
    ],
)

bzd_nodejs_library(
    name = "mocha",
    packages = {
        "mocha": "",
        "c8": "",
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
        "//tools/bazel_build:binary_wrapper",
        "@bazel_skylib//rules:common_settings",
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
        ":package",
        "//tools/bazel_build:binary_wrapper",
        "//tools/bazel_build/rules/third_party/cc:bzl",
        "@bazel_skylib//lib:sets",
        "@bazel_skylib//rules:common_settings",
    ],
)

bzl_library(
    name = "bdl",
    srcs = [
        "bdl.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//tools/bazel_build/rules/assets/cc:defs",
        "@bazel_skylib//lib:sets",
    ],
)
