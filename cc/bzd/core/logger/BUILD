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
        ":minimal",
        "//cc/bzd/core/logger/backend",
        "//cc/bzd/platform:clock",
        "//cc/bzd/utility/string/to_stream:format",
    ],
)

cc_library(
    name = "minimal",
    hdrs = [
        "minimal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:source_location",
    ],
)
