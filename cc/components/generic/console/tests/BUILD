load("@bzd_bdl//:defs.bzl", "bdl_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/components/generic/console",
    ],
)

bzd_cc_binary(
    name = "tests",
    deps = [
        ":composition",
    ],
)
