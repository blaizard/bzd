load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:timer",
    ],
)

cc_library(
    name = "xthal",
    srcs = [
        "xthal.cc",
    ],
    hdrs = [
        "xthal.hh",
    ],
    target_compatible_with = [
        "@bzd_platforms//isa:xtensa_lx6",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/core/async",
    ],
)
