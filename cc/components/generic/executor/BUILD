load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:core",
        "//interfaces:executor",
    ],
)

cc_library(
    name = "executor",
    hdrs = [
        "executor.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/container:function_ref",
        "//cc/bzd/core/logger",
        "//cc/bzd/utility:apply",
    ],
)
