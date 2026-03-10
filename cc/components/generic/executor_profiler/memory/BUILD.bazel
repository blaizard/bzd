load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
)

cc_library(
    name = "memory",
    hdrs = [
        "memory.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/container:variant",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/core/async:executor_profiler",
    ],
)
