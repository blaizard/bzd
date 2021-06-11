load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bzd_manifest")

bzd_manifest(
    name = "bdl",
    srcs = [
        "linux.bdl",
    ],
    deps = [
        "//cc/bzd/platform:core",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "linux",
    hdrs = [
        "core.h",
    ],
    linkopts = [
        "-lpthread",
    ],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/platform/core:stack",
        ":bdl",
    ],
)

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":linux",
    ],
)
