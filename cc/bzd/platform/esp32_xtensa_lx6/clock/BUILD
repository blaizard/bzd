load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "clock",
    srcs = [
        "clock.cc",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:clock",
    ],
    alwayslink = True,
)
