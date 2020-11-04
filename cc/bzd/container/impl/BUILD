load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "single_linked_pool",
    hdrs = [
        "single_linked_pool.h",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/core:assert_minimal",
        "//cc/bzd/platform:types",
    ],
)
