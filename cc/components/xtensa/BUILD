load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "error",
    hdrs = [
        "error.hh",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/core:error",
    ],
    target_compatible_with = [
        "@//tools/bazel_build/platforms/isa:xtensa_lx6",
    ],
)
