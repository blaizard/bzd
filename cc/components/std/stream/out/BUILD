load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//interfaces:stream",
    ],
)

cc_library(
    name = "out",
    hdrs = [
        "out.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/core:channel",
    ],
)
