load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:io",
    ],
)

cc_library(
    name = "stub",
    hdrs = [
        "stub.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
    ],
)
