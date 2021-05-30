load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "linux",
    hdrs = [
        "core.h",
    ],
    linkopts = [
        "-lpthread",
    ],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/platform:core",
        "//cc/bzd/platform/core:stack",
    ],
)

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":linux",
    ],
)
