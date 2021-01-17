load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary", "cc_library")

cc_library(
    name = "lib",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//cc/bzd",
    ],
)

bzd_cc_binary(
    name = "format",
    deps = [
        ":lib",
    ],
)
