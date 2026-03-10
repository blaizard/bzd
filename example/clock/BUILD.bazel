load("@bzd_bdl//:defs.bzl", "bdl_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//interfaces:stream",
        "//interfaces:timer",
    ],
)

bzd_cc_binary(
    name = "clock",
    hdrs = [
        "counter.hh",
    ],
    deps = [
        ":composition",
        "//cc/bzd/core:print",
    ],
)
