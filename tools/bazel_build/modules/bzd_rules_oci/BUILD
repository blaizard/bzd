load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("//:private/crane_binary.bzl", "bzd_crane_binary")

exports_files(["defs.bzl"])

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
