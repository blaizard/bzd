load("@rules_cc//cc:defs.bzl", "cc_library")
load("@bazel_skylib//lib:selects.bzl", "selects")

cc_library(
    name = "bootstrap",
    srcs = selects.with_or({
        (
            "//tools/bazel_build/config:x86_64",
        ): ["bootstrap_x86_64.cc"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    alwayslink = True,
)
