load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
)

cc_library(
    name = "xthal_clock",
    srcs = [
        "xthal_clock.cc",
    ],
    hdrs = [
        "xthal_clock.hh",
    ],
    target_compatible_with = [
        "@esp32_xtensa_lx6_gcc//:compiler",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/core:clock",
    ],
)
