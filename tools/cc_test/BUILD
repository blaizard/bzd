load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cc_test",
    srcs = [
        "test.cc",
    ],
    hdrs = [
        "test.hh",
        "test_internal.hh",
    ],
    include_prefix = "cc_test",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/assert",
        "//cc/bzd/platform",
    ],
    alwayslink = True,
)
