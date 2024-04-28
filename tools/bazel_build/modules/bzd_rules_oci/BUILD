load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@bzd_rules_oci//:defs.bzl", "bzd_oci_images_config")
load("@bzd_rules_oci//:private/crane_binary.bzl", "bzd_crane_binary")

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
