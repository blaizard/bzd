load("@bzd_bdl//:defs.bzl", "bdl_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
)

bzd_cc_binary(
    name = "pi",
    srcs = [
        "pi.hh",
    ],
    tags = ["manual"],
    deps = [
        #":composition",
        "//cc/bzd/core/async",
    ],
)
