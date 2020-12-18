load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cc_test",
    srcs = [
        "test.cc",
    ],
    hdrs = [
        "test.h",
        "test_internal.h",
    ],
    include_prefix = "cc_test",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform",
    ],
    alwayslink = True,
)

cc_library(
    name = "sync_points",
    hdrs = [
        "sync_points.h",
    ],
    include_prefix = "cc_test",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform",
    ],
)
