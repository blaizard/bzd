load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "executor.bdl",
    ],
    deps = [
        "//cc/bzd/platform:core",
        "//cc/bzd/platform:executor",
    ],
)

cc_library(
    name = "executor",
    hdrs = [
        "executor.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/utility:constexpr_for",
    ],
)
