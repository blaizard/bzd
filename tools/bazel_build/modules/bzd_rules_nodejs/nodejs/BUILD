load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load(":defs.bzl", "bzd_nodejs_library")

exports_files([
    "defs.bzl",
])

toolchain_type(
    name = "toolchain_type",
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "mocha",
    packages = {
        "c8": "",
        "mocha": "",
    },
    visibility = ["//visibility:public"],
)

string_flag(
    name = "cache",
    build_setting_default = "",
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
        "@bzd_package//:defs",
        "@rules_oci//oci:defs",
    ],
)
