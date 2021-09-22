load("@//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "stub",
    hdrs = [
        "stub.hh",
    ],
    includes = [
        "libraries/io/stub/stub.hh",
    ],
    interfaces = {
        "bzd::io::Stub": "*",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
    ],
)
