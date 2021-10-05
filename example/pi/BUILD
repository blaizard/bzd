load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
)

bzd_cc_binary(
    name = "pi",
    srcs = [
        "pi.hh",
    ],
    tags = ["manual"],
    deps = [
        #":composition",
        "//cc/bzd/core/async",
    ],
)
