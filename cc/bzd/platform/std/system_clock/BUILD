load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "system_clock",
    srcs = [
        "system_clock.cc",
    ],
    hdrs = [
        "system_clock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:clock",
    ],
)
