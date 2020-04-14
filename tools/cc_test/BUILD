load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cc_test",
    srcs = [
        "test.cpp",
    ],
    hdrs = [
        "test.h",
        "test_internal.h",
    ],
    include_prefix = "cc_test",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/libraries/bootstrap",
    ],
    alwayslink = True,
)
