load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/platform/interfaces:stream",
        "//cc/bzd/platform/interfaces:timer",
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
