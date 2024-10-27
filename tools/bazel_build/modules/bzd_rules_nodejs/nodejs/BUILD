load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("//nodejs:toolchain.bzl", "nodejs_manager_binary")

exports_files([
    "defs.bzl",
])

toolchain_type(
    name = "toolchain_type",
    visibility = ["//visibility:public"],
)

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bazel_skylib//rules:common_settings",
        "@bzd_lib//:sh_binary_wrapper",
        "@bzd_lib//lib:attrs",
        "@bzd_package//:defs",
        "@bzd_rules_oci//:defs",
    ],
)

nodejs_manager_binary(
    name = "pnpm",
    visibility = ["//visibility:public"],
)
