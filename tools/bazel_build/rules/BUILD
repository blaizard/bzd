load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

exports_files([
    "nodejs.bzl",
    "nodejs_web.bzl",
    "package.bzl",
])

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
        "//tools/bazel_build/settings/cache:defs",
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
    name = "manifest",
    srcs = [
        "manifest.bzl",
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
        ":manifest",
    ],
)

bzl_library(
    name = "bdl",
    srcs = [
        "bdl.bzl",
    ],
    visibility = ["//visibility:public"],
)
