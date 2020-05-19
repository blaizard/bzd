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
