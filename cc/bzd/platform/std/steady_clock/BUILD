load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "steady_clock",
    srcs = [
        "steady_clock.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:clock",
    ],
    alwayslink = True,
)
