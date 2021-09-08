load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "logger",
    srcs = [
        "logger.cc",
    ],
    hdrs = [
        "logger.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/logger/backend",
    ],
)
