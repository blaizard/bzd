load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "format",
    hdrs = [
        "format.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/utility/string:base_async",
        "//cc/bzd/utility/string/to_string:format",
    ],
)
