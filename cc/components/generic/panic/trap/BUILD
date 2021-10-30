load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "trap",
    srcs = [
        "panic.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:panic",
    ],
    alwayslink = True,
)
