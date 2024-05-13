load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/components/posix/proactor",
    ],
)

cc_library(
    name = "mock",
    hdrs = [
        "proactor.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
    ],
)
