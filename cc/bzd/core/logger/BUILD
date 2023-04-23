load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "logger",
    srcs = [
        "logger.cc",
    ],
    hdrs = [
        "//cc/bzd/core:logger.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility/pattern/formatter:to_stream",
    ],
)
