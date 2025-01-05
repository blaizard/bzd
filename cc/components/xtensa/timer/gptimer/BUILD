load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:timer",
    ],
)

cc_library(
    name = "gptimer",
    srcs = [
        "gptimer.cc",
    ],
    hdrs = [
        "gptimer.hh",
    ],
    target_compatible_with = select({
        "@bzd_platforms//isa:xtensa_lx6": [],
        "@bzd_platforms//isa:xtensa_lx7": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/core:print",
        "//cc/bzd/core/async",
        "//cc/components/xtensa:error",
        "//cc/libs/timer:timer_isr",
    ],
)
