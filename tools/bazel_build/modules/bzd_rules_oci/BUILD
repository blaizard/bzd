load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("//:defs.bzl", "bzd_oci_images_config")
load("//:private/crane_binary.bzl", "bzd_crane_binary")

exports_files(["defs.bzl"])

bzd_oci_images_config(
    name = "config",
    out = "config.json",
    visibility = ["//visibility:public"],
)

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@rules_oci//oci:pull",
    ],
)

bzd_crane_binary(
    name = "crane",
    visibility = ["//visibility:public"],
)
