load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "tcp",
    srcs = [
        "interface.bdl",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//interfaces:tcp",
    ],
)

cc_library(
    name = "client",
    hdrs = [
        "client.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":tcp",
        "//cc/bzd/core/assert",
    ],
)
