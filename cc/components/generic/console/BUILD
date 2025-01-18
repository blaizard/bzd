load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
)

cc_library(
    name = "console",
    hdrs = [
        "console.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc:bzd",
    ],
)
