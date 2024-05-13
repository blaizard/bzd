load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:core",
    ],
)

cc_library(
    name = "core",
    hdrs = [
        "core.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:stack",
        "//cc/bzd/core:error",
        "//cc/bzd/core:print",
        "//cc/bzd/math:ceil",
        "//cc/bzd/utility:align_up",
        "//cc/components/posix:error",
        "//cc/libs/pthread",
    ],
)
