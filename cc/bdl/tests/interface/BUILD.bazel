load("@bzd_bdl//:defs.bzl", "bdl_library")
load("//cc/bdl:cc.bzl", "bzd_cc_test")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
)

bzd_cc_test(
    name = "cc",
    srcs = [
        "implementation.cc",
    ],
    deps = [
        ":interface",
        "//cc/bzd/test",
    ],
)
