load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
)

cc_library(
    name = "timer",
    srcs = [
        "timer.cc",
    ],
    hdrs = [
        "timer.hh",
    ],
    target_compatible_with = [
        "@//tools/bazel_build/platforms/isa:xtensa_lx6",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/core:clock",
        "//cc/bzd/core:print",
        "//cc/bzd/core/async",
        "//cc/components/xtensa:error",
        "//cc/libs/timer:timer_isr",
    ],
)
