load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "proactor",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":proactor_lib",
    ],
)

cc_library(
    name = "proactor_lib",
    hdrs = [
        "proactor.hh",
    ],
    deps = [
        "//cc/bzd/core/async",
        "//cc/components/posix/io:file_descriptor",
        "//cc/components/posix/network/address",
    ],
)
