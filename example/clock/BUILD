load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//interfaces:stream",
        "//interfaces:timer",
    ],
)

bzd_cc_binary(
    name = "clock",
    hdrs = [
        "counter.hh",
    ],
    deps = [
        ":composition",
    ],
)
