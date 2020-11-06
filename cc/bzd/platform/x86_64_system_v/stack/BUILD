load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "stack",
    srcs = [
        "stack.S",
        "stack.cc",
    ],
    tags = ["manual"],
    visibility = ["//cc/bzd/platform:__pkg__"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
)
