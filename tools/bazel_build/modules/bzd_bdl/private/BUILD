load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

bzl_library(
    name = "defs",
    srcs = [
        "application_factory.bzl",
        "common.bzl",
        "library.bzl",
        "providers.bzl",
        "system.bzl",
        "target.bzl",
    ],
    visibility = ["//:__pkg__"],
    deps = [
        "//private/extensions:json",
        "@@protobuf+//bazel:cc_proto_library_bzl",  # Not sure why this is needed:
        "@bazel_skylib//lib:sets",
        "@bazel_skylib//rules:common_settings",
        "@bdl_extension//:extensions",
        "@bzd_lib//:sh_binary_wrapper",
        "@bzd_lib//config:defs",
        "@bzd_package//:defs",
        "@bzd_toolchain_cc//cc:defs",
        "@rules_cc//cc:defs_bzl",
        "@rules_cc//cc:find_cc_toolchain_bzl",
    ],
)
