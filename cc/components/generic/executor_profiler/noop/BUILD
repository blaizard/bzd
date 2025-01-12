load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
)

cc_library(
    name = "core_profiler",
    srcs = [
        "core_profiler.cc",
    ],
    hdrs = [
        "core_profiler.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async:executor_profiler",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "noop",
    hdrs = [
        "noop.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        ":core_profiler",
        "//cc/bzd/core/async:executor_profiler",
        "//cc/bzd/platform:types",
    ],
)
