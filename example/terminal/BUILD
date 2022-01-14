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
    name = "reader",
    hdrs = [
        "reader.hh",
    ],
    visibility = [
        "//example/terminal/tests:__pkg__",
    ],
    deps = [
        "//cc:bzd",
    ],
)

cc_library(
    name = "lib",
    srcs = [
        "terminal.cc",
    ],
    hdrs = [
        "terminal.hh",
    ],
    deps = [
        ":composition",
        ":reader",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "terminal",
    deps = [
        ":lib",
    ],
)
