load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "common",
    srcs = [
        "common.bdl",
    ],
)

cc_library(
    name = "lib",
    hdrs = [
        "simple.hh",
    ],
    deps = [
        ":common",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "1_emitter",
    bdls = [
        "1_emitter.bdl",
    ],
    deps = [
        ":lib",
    ],
)

bzd_cc_binary(
    name = "1_emitter_2_pollers",
    bdls = [
        "1_emitter_2_pollers.bdl",
    ],
    deps = [
        ":lib",
    ],
)
