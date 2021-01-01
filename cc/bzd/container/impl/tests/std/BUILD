load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_library(
    name = "circular_list",
    srcs = [
        "circular_list.cc",
    ],
    linkopts = [
        "-pthread"
    ],
    deps = [
        "//cc/bzd/container/impl",
        "//tools/cc_test",
        "//cc/bzd/test:sync_point",
    ],
    visibility = ["//visibility:public"],
)
