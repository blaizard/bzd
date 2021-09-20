load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "backend",
    srcs = [
        "logger.cc",
    ],
    hdrs = [
        "logger.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
    ],
)
