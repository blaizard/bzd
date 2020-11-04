load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "context",
    srcs = [
        "context.S",
        "context.cc",
    ],
    tags = ["manual"],
    visibility = ["//cc/bzd/platform:__pkg__"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
)
