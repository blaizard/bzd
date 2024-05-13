load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:core",
    ],
)

cc_library(
    name = "core",
    hdrs = [
        "core.hh",
    ],
    target_compatible_with = [
        "@bzd_platforms//isa:xtensa_lx6",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:stack",
        "//cc/bzd/core:error",
    ],
)
