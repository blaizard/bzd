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
    name = "lib",
    srcs = [
        "jardinier.cc",
    ],
    hdrs = [
        "jardinier.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "jardinier2000",
    deps = [
        ":lib",
    ],
)
