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
    name = "steady_clock",
    srcs = [
        "steady_clock.cc",
    ],
    hdrs = [
        "steady_clock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
    ],
)
