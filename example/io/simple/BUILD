load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "common",
    srcs = [
        "common.bdl",
    ],
)

cc_library(
    name = "lib",
    hdrs = [
        "simple.hh",
    ],
    deps = [
        ":common",
        "//cc:bzd",
    ],
)

bdl_library(
    name = "1_emitter_bdl",
    srcs = [
        "1_emitter.bdl",
    ],
)

bzd_cc_binary(
    name = "1_emitter",
    deps = [
        ":1_emitter_bdl",
        ":lib",
    ],
)

bdl_library(
    name = "1_emitter_2_pollers_bdl",
    srcs = [
        "1_emitter_2_pollers.bdl",
    ],
)

bzd_cc_binary(
    name = "1_emitter_2_pollers",
    deps = [
        ":1_emitter_2_pollers_bdl",
        ":lib",
    ],
)
