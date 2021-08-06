load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bzd_manifest")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")

bzd_manifest(
    name = "bdl",
    srcs = [
        "linux.bdl",
        "main.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:core",
    ],
)

cc_library(
    name = "linux",
    hdrs = [
        "core.h",
        "executor.h",
    ],
    linkopts = [
        "-lpthread",
    ],
    deps = [
        ":bdl",
        "//cc/bzd/container:optional",
        "//cc/bzd/platform/core:stack",
        "//cc/bzd/utility:constexpr_for",
    ],
)

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
        "pi.h",
    ],
    deps = [
        ":linux",
        "//cc/bzd/core:async",
        "//cc/bzd/core:delay",
    ],
)

bzd_cc_binary(
    name = "app",
    srcs = [
        "main.cc",
        "pi.h",
    ],
    deps = [
        ":linux",
        "//cc/bzd/core:async",
        "//cc/bzd/core:delay",
    ],
)
