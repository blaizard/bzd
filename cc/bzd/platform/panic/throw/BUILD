load("//tools/bazel_build/rules:cc.bzl", "cc_library")

config_setting(
    name = "backend",
    define_values = {
        "panic": "throw",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "throw",
    srcs = [
        "throw.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
    alwayslink = True,
)
