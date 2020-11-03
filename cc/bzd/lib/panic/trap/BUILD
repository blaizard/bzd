load("//tools/bazel_build/rules:cc.bzl", "cc_library")

config_setting(
    name = "backend",
    define_values = {
        "panic": "trap",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "trap",
    srcs = [
        "panic.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:system_header",
    ],
    alwayslink = True,
)
