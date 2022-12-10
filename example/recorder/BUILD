load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
)

cc_library(
    name = "ids",
    srcs = [
        "ids.cc",
    ],
    deps = [
        "//cc:bzd",
    ],
)

cc_library(
    name = "lib",
    srcs = [
        "recorder.cc",
    ],
    hdrs = [
        "recorder.hh",
    ],
    deps = [
        ":composition",
        ":ids",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "recorder",
    deps = [
        ":lib",
    ],
)
