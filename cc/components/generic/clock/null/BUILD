load("@rules_cc//cc:defs.bzl", "cc_library")
load("@bzd_bdl//:defs.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:clock",
    ],
)

cc_library(
    name = "null",
    srcs = [
        "null.cc",
    ],
    hdrs = [
        "null.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
    ],
)
